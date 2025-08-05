savedcmd_module_params.mod := printf '%s\n'   module_params.o | awk '!x[$$0]++ { print("./"$$0) }' > module_params.mod
