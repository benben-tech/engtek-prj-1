Import("env", "projenv")
from shutil import copyfile

def save_bin(*args, **kwargs):
  target = str(kwargs['target'][0])
  saveName = target.split('\\')[-1]
  saveEnv = target.split('\\')[-2]
  saveFile = 'binaries\{}\{}'.format(saveEnv, saveName)
  copyfile(target, saveFile)

def save_elf(*args, **kwargs):
  target = str(kwargs['target'][0])
  saveName = target.split('\\')[-1]
  saveEnv = target.split('\\')[-2]
  saveFile = 'binaries\{}\{}'.format(saveEnv, saveName)
  copyfile(target, saveFile)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", save_bin)
env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", save_elf)