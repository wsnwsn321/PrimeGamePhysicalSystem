
import os
import sys
import shutil
import stat

sys.path.append(os.path.normpath(os.path.join(os.environ["PE_WORKSPACE_DIR"], "Tools")))
#print sys.path
import PyClient
import PyClient.assetmanagercommon

#files in Pyengine2 folder
root = os.path.abspath(os.environ["PE_WORKSPACE_DIR"]) # this is a path to the workspace directory. 
dest = os.path.abspath(os.path.join(root, "..", "Branch"))

def defaultFilterFunc(srcDir, relDir, name):
    print "relDir:", relDir
    if relDir == '.':
        #PEWorkspace
        if name == "3rdPartyInstallations" or \
        name == "AssetsIn" or \
        name == "AssetsOut" or \
        name == "Code" or \
        name == "External" or \
        name == "GlobalConfig" or \
        name == "Python25" or \
        name == "Python26" or \
        name == "Tools":
            pass
        else:
            return False
    if relDir == "3rdPartyInstallations":
        if name == "XBox360":
            return False
            
    if relDir == "Code":
        # directories
        if name == "Atg" \
        or name == "Build" \
        or name == ".vs" \
        or name == "SampleApp" \
        or name == "Debug" \
        or name == "Release" \
        or name == "LUA" \
        or name == "SampleWorkspace.xcodeproj" \
        or name == "Doxyfile":
            return False
    
    if relDir == "Tools":
        # directories
        if name == "tray" \
        or name == "SRV":
            return False
    
    if relDir == "AssetsIn\\XFiles":
        return False
    
    if relDir == "External\\DownloadedLibraries\\Cg":
        if name == "examples":
            return False
    
    
    ext = os.path.splitext(name)[1]
    if ext == ".vcxproj" \
    or ext == ".user" \
    or ext == ".filters" \
    or ext == ".sln" \
    or ext == ".sdf" \
    or ext == ".suo" \
    or ext == ".mm" \
    or ext == ".props" \
    or ext == ".rules" \
    or ext == ".targets" \
    or ext == ".xml":
        return False
    
    
    
    
    
    print name, "OK"
    return True
    
def copyDir(srcDir, destParent, level):
    print "copying", srcDir, "into", destParent
    dirName = os.path.basename(srcDir)
    destDir = os.path.join(destParent, dirName)
    print "making dir", destDir
    os.mkdir(destDir)
    
    names = os.listdir(srcDir)
    
    filterFunc = defaultFilterFunc
    relSrcDir = os.path.relpath(srcDir, root)
    names = filter(lambda x: defaultFilterFunc(srcDir, relSrcDir, x), names)
    
    for name in names:
        existingPath = os.path.join(srcDir, name)
        if os.path.isdir(existingPath):
            print name, "is directory, copying.."
            if level < 5:
                copyDir(existingPath, destDir, level + 1)
            
        elif os.path.isfile(existingPath):
            print name, "is file, copying.."
            shutil.copy(existingPath, destDir)
            os.chmod( os.path.join(destDir, name), stat.S_IWRITE )
    
def main():
    print "branch-no-xbox: Destination folder:", dest
    if not os.path.exists(dest):
        print "Destination doesn't exist"
    elif os.path.isdir(dest):
        print "Deleting folder", dest
        shutil.rmtree(dest)
    else:
        print "Error: Destination", dest, "exists, but is not a folder"
        return
        
    if not os.path.exists(dest):
        print "Creating destination folder"
        os.mkdir(dest)
    else:
        print "Error: Destination Folder Exists and wasn't deleted", dest
        return
        
    copyDir(root, dest, 1)
    return
    
    print 'Have request to deploy package', package
    #use common API to retrieve list of packages
    availPackages = PyClient.assetmanagercommon.getListOfAvailablePackages()
    if not package in availPackages:
        print 'PYENGINE: TOOLS: ERROR: package %s was not found in current PyengineWorkspace' % (package,)
        return
    availTypesInPackage = PyClient.assetmanagercommon.getListOfAvailableAssetTypesForPacakge(package)
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    
    platform = "XBox360" #default target platform (other option might be IOS)
    
    if len(sys.argv) > 2:
        platform = sys.argv[2]

    print 'Deployment platform:', platform
    if platform == 'IOS':
        if (len(sys.argv) < 4):
            print 'PYENGINE: TOOLS: ERROR: not target path provided fro IOS option. Dont know where to deploy files.'
            return 
        targetWorkspace = sys.argv[3]
    else:
        targetWorkspace = conf['X360WorkspacePath']
    
    print "Available asset types in the %s package" % (package,)
    print availTypesInPackage
    for t in availTypesInPackage:
        targetAssetPath = os.path.join(targetWorkspace ,'AssetsOut', package, t)
        files = PyClient.assetmanagercommon.getListOfAvailableAssetsForPacakgeOfType(package, t)
        for f in files:
            filePath = PyClient.assetmanagercommon.getAssetFullPath(package, t, f)
            fullTargetPath = os.path.join(targetAssetPath, f)
            print "Try to copy file:", filePath
            print 'to'
            print fullTargetPath
            if platform == 'IOS':
                os.system('mkdir -p "%s"' % os.path.split(fullTargetPath)[0])
                os.system('cp -f "%s" "%s"' % (filePath, fullTargetPath))
            else:
                os.system('XbCp /d /Y /t "%s" "%s"' % (filePath, fullTargetPath))
            
if __name__ == "__main__":
    main()
