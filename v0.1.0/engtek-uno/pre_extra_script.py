Import("env")

build_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in build_flags.get("CPPDEFINES")}

device_type = str(defines.get("DEVICE_TYPE")).replace('"', "")

env.Replace(PROGNAME="%s" %(device_type))