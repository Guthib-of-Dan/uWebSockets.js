// https://github.com/<owner>/<repo>/releases/download/<tag>/<filename>
var fs = require("node:fs/promises")
var repo = "Guthib-of-Dan/uWebSockets.js"
var link = "https://github.com/" + repo + "/releases/download/" +
  /*TAG*/ "v0.0.0"
  + "/";
var requiredFilename = 'uws_' + process.platform + '_' + process.arch + '_' + process.versions.modules + '.node'
fetch(link + requiredFilename)
  .then((res)=>res.arrayBuffer())
  .then((buffer)=>fs.writeFile(requiredFilename, buffer))
