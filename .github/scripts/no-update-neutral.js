module.exports = async ({ core }) => {
  const currentTag = process.env.CURRENT_TAG || 'unknown';
  core.notice(`No vendored Mongoose update needed. Current tag: ${currentTag}`);
  core.setNeutral('No upstream Mongoose version change detected.');
};
