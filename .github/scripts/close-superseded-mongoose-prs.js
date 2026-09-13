module.exports = async ({ github, context }) => {
  const owner = context.repo.owner;
  const repo = context.repo.repo;
  const latestTag = process.env.LATEST_TAG;
  const currentBranch = `ci/update-mongoose-${latestTag}`;
  const currentPrNumber = Number(process.env.CURRENT_PR_NUMBER || '0');
  const currentPrUrl = process.env.CURRENT_PR_URL || '';
  const replacementRef = currentPrUrl || (currentPrNumber > 0 ? `#${currentPrNumber}` : 'the latest updater PR');

  const openPrs = await github.paginate(github.rest.pulls.list, {
    owner,
    repo,
    state: 'open',
    base: 'mongoose_7',
    per_page: 100,
  });

  for (const pr of openPrs) {
    if (!pr.head.ref.startsWith('ci/update-mongoose-')) continue;
    if (pr.head.ref === currentBranch) continue;
    if (currentPrNumber > 0 && pr.number === currentPrNumber) continue;

    await github.rest.issues.createComment({
      owner,
      repo,
      issue_number: pr.number,
      body: `Closing in favor of ${replacementRef}, which updates vendored Mongoose to \`${latestTag}\`.`,
    });

    await github.rest.pulls.update({
      owner,
      repo,
      pull_number: pr.number,
      state: 'closed',
    });
  }
};
