const fs = require("fs").promises;
const { createHash } = require("node:crypto");

module.exports = async ({ github, context }) => {
  const VERSION = process.env.GITHUB_REF_NAME;
  const { owner, repo } = context.repo;

  const compiled_extensions = [
    {
      path: `sqlite-assert-macos-x86_64-extension/assert0.dylib`,
      name: `sqlite-assert-${VERSION}-deno-darwin-x86_64.assert0.dylib`,
    },
    {
      path: `sqlite-assert-macos-aarch64-extension/assert0.dylib`,
      name: `sqlite-assert-${VERSION}-deno-darwin-aarch64.assert0.dylib`,
    },
    {
      path: `sqlite-assert-linux-x86_64-extension/assert0.so`,
      name: `sqlite-assert-${VERSION}-deno-linux-x86_64.assert0.so`,
    },
    {
      path: `sqlite-assert-windows-x86_64-extension/assert0.dll`,
      name: `sqlite-assert-${VERSION}-deno-windows-x86_64.assert0.dll`,
    },
  ];

  const release = await github.rest.repos.getReleaseByTag({
    owner,
    repo,
    tag: VERSION,
  });
  const release_id = release.data.id;
  const outputAssetChecksums = [];

  await Promise.all(
    compiled_extensions.map(async ({ name, path }) => {
      const data = await fs.readFile(path);
      const checksum = createHash("sha256").update(data).digest("hex");
      outputAssetChecksums.push({ name, checksum });
      return github.rest.repos.uploadReleaseAsset({
        owner,
        repo,
        release_id,
        name,
        data,
      });
    })
  );

  return outputAssetChecksums.map((d) => `${d.checksum} ${d.name}`).join("\n");
};
