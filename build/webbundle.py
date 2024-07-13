Import("env")

# Install the necessary node packages for the pre-build asset bundling script
env.Execute("npm install")

# Call the bundling script
exitCode = env.Execute("node build/webbundle.mjs")

# If it failed, abort the build
if (exitCode):
  Exit(exitCode)
