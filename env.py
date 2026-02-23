import os
Import("env")

env_file = ".env"

if os.path.exists(env_file):
    with open(env_file, "r") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            
            key, value = line.strip().split("=", 1)
            
            # REMOVE existing quotes from .env to avoid triple-quoting
            clean_value = value.strip('"').strip("'")
            
            # ADD escaped quotes so C++ sees a string literal
            # This turns Armorica into "Armorica" for the compiler
            env.Append(CPPDEFINES=[(key, f'\\"{clean_value}\\"')])
            
            print(f"Injected: {key} as string")