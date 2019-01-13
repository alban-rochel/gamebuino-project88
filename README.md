Reach 88 mph before running out of fuel!

To compile for appropriate performance, change:
C:\Users\<User>\AppData\Local\Arduino15\packages\gamebuino\hardware\samd\1.2.1\platform.txt
Remove the -Os and -g flags:
compiler.c.flags=-mcpu={build.mcu} -mthumb -c -O3 {compiler.warning_flags} -std=gnu11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -MMD -D__SKETCH_NAME__="""{build.project_name}"""
compiler.cpp.flags=-mcpu={build.mcu} -mthumb -c -O3 {compiler.warning_flags} -std=gnu++11 -ffunction-sections -fdata-sections -fno-threadsafe-statics -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -MMD -D__SKETCH_NAME__="""{build.project_name}"""
