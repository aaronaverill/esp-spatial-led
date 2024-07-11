Import("env")

exitCode = env.Execute("node build/webbundle.mjs")
if (exitCode):
  Exit(exitCode)
