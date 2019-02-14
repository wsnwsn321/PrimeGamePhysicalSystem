--
-- Embed the Lua scripts into src/host/scripts.c as static data buffers.
-- Embeds minified versions of the actual scripts by default, rather than
-- bytecode, as bytecodes are not portable to different architectures. Use
-- the `--bytecode` flag to override.
--

	local scriptCount = 0

	local function loadScript(fname)
		fname = path.getabsolute(fname)
		local f = io.open(fname, "rb")
		local s = assert(f:read("*all"))
		f:close()
		return s
	end


	local function stripScript(s)
		-- strip tabs
		local result = s:gsub("[\t]", "")

		-- strip any CRs
		result = result:gsub("[\r]", "")

		-- strip out block comments
		result = result:gsub("[^\"']%-%-%[%[.-%]%]", "")
		result = result:gsub("[^\"']%-%-%[=%[.-%]=%]", "")
		sresult = result:gsub("[^\"']%-%-%[==%[.-%]==%]", "")

		-- strip out inline comments
		result = result:gsub("\n%-%-[^\n]*", "\n")

		-- strip duplicate line feeds
		result = result:gsub("\n+", "\n")

		-- strip out leading comments
		result = result:gsub("^%-%-[^\n]*\n", "")

		return result
	end


	local function outputScript(result, script)
		local data   = script.data
		local length = #data

		if length > 0 then
			script.table = string.format("builtin_script_%d", scriptCount)
			scriptCount = scriptCount + 1

			buffered.writeln(result, "// ".. script.name)
			buffered.writeln(result, "static const unsigned char " .. script.table .. "[] = {")

			for i = 1, length do
				buffered.write(result, string.format("%3d, ", data:byte(i)))
				if (i % 32 == 0) then
					buffered.writeln(result)
				end
			end

			buffered.writeln(result, "};")
			buffered.writeln(result)
		end
	end


	local function addScript(result, filename, name, data)
		if not data then
			if _OPTIONS["bytecode"] then
				verbosef("Compiling... " .. filename)
				local output = path.replaceextension(filename, ".luac")
				local res, err = os.compile(filename, output);
				if res ~= nil then
					data = loadScript(output)
					os.remove(output)
				else
					print(err)
					print("Embedding source instead.")
					data = stripScript(loadScript(filename))
				end
			else
				data = stripScript(loadScript(filename))
			end
		end

		local script = {}
		script.filename = filename
		script.name     = name
		script.data     = data
		table.insert(result, script)
	end


-- Prepare the file header

	local result = buffered.new()
	buffered.writeln(result, "/* Premake's Lua scripts, as static data buffers for release mode builds */")
	buffered.writeln(result, "/* DO NOT EDIT - this file is autogenerated - see BUILD.txt */")
	buffered.writeln(result, "/* To regenerate this file, run: premake5 embed */")
	buffered.writeln(result, "")
	buffered.writeln(result, '#include "host/premake.h"')
	buffered.writeln(result, "")

-- Find all of the _manifest.lua files within the project

	local mask = path.join(_MAIN_SCRIPT_DIR, "**/_manifest.lua")
	local manifests = os.matchfiles(mask)

-- Find all of the _user_modules.lua files within the project

	local userModuleFiles = {}
	userModuleFiles = table.join(userModuleFiles, os.matchfiles(path.join(_MAIN_SCRIPT_DIR, "**/_user_modules.lua")))
	userModuleFiles = table.join(userModuleFiles, os.matchfiles(path.join(_MAIN_SCRIPT_DIR, "_user_modules.lua")))


-- Generate table of embedded content.
	local contentTable = {}

	print("Compiling... ")
	for mi = 1, #manifests do
		local manifestName = manifests[mi]
		local manifestDir  = path.getdirectory(manifestName)
		local baseDir      = path.getdirectory(manifestDir)

		local files = dofile(manifests[mi])
		for fi = 1, #files do
			local filename = path.join(manifestDir, files[fi])
			addScript(contentTable, filename, path.getrelative(baseDir, filename))
		end
	end

	addScript(contentTable, path.join(_SCRIPT_DIR, "../src/_premake_main.lua"), "src/_premake_main.lua")
	addScript(contentTable, path.join(_SCRIPT_DIR, "../src/_manifest.lua"), "src/_manifest.lua")

-- Add the list of modules

	local modules = dofile("../src/_modules.lua")
	for _, userModules in ipairs(userModuleFiles) do
		modules = table.join(modules, dofile(userModules))
	end

	addScript(contentTable, "_modules.lua", "src/_modules.lua", "return {" .. table.implode(modules, '"', '"', ', ') .. "}")

-- Embed the actual script contents

	print("Embedding...")
	for mi = 1, #contentTable do
		outputScript(result, contentTable[mi])
	end

-- Generate an index of the script file names. Script names are stored
-- relative to the directory containing the manifest, i.e. the main
-- Xcode script, which is at $/modules/xcode/xcode.lua is stored as
-- "xcode/xcode.lua".
	buffered.writeln(result, "const buildin_mapping builtin_scripts[] = {")

	for mi = 1, #contentTable do
		if contentTable[mi].table then
			buffered.writeln(result, string.format('\t{"%s", %s, sizeof(%s)},', contentTable[mi].name, contentTable[mi].table, contentTable[mi].table))
		else
			buffered.writeln(result, string.format('\t{"%s", NULL, 0},', contentTable[mi].name))
		end
	end

	buffered.writeln(result, "\t{NULL, NULL, 0}")
	buffered.writeln(result, "};")
	buffered.writeln(result, "")

-- Write it all out. Check against the current contents of scripts.c first,
-- and only overwrite it if there are actual changes.

	print("Writing...")
	local scriptsFile = path.getabsolute(path.join(_SCRIPT_DIR, "../src/scripts.c"))
	local output = buffered.tostring(result)

	local f, err = os.writefile_ifnotequal(output, scriptsFile);
	if (f < 0) then
		error(err, 0)
	elseif (f > 0) then
		printf("Generated %s...", path.getrelative(os.getcwd(), scriptsFile))
	end
