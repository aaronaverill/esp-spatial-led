Import("env")

exitCode = env.Execute("node build/webbundle.js")
if (exitCode):
  Exit(exitCode)
