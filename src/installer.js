// https://github.com/<owner>/<repo>/releases/download/<tag>/<filename>
var repo = "Guthib-of-Dan/uWebSockets.js"
var tag = "v0.66.02"

module.exports = async function downloadBinary(paramTag = tag, paramFilename = 'uws_' + process.platform + '_' + process.arch + '_' + process.versions.modules + '.node') {
  var execSync = require("node:child_process").execSync
  var archiveName = paramFilename + ".tar.gz";
  var link = "https://github.com/" + repo + "/releases/download/" + paramTag + "/" + archiveName;

  console.log("Expected binary - " + paramFilename);
  execSync(`curl -sS '${link}' -o ${archiveName} && tar -xzf ${paramFilename} ${archiveName} && rm ${archiveName}`, {stdio: "inherit", encoding: "utf16le"}, ); 

  console.log("finished")
}

if (process.argv[2] == "default") {
  module.exports();
}
