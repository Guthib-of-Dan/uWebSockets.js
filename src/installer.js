// https://github.com/<owner>/<repo>/releases/download/<tag>/<filename>
var fs = require("node:fs/promises")
var repo = "Guthib-of-Dan/uWebSockets.js"
var tag = "v0.0.0"
export default async function downloadBinary(paramTag = tag, paramFilename = 'uws_' + process.platform + '_' + process.arch + '_' + process.versions.modules + '.node') {
  var link = "https://github.com/" + repo + "/releases/download/" + paramTag + "/";

  console.log("Expected binary - " + paramFilename);

  var res = await fetch(link + paramFilename)
  var buffer = await res.arrayBuffer()
  await fs.writeFile(paramFilename, buffer)
  console.log("finished")
}

if (process.argv[2] == "default") {
  downloadBinary();
}
