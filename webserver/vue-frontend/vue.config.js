const path = require("path");

module.exports = {
  // Changed so that the build step builds into the node server's public folder
  outputDir: path.resolve(__dirname, "../public")
};
