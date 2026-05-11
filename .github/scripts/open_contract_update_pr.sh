#!/usr/bin/env bash
set -euo pipefail

TAG="${TAG:?Missing TAG}"
COMMIT="${COMMIT:?Missing COMMIT}"
CLASSIFICATION="${CLASSIFICATION:?Missing CLASSIFICATION}"

BRANCH="chore/vendor-telemetry-contract-${TAG}"
git config user.name "github-actions[bot]"
git config user.email "41898282+github-actions[bot]@users.noreply.github.com"

git checkout -B "${BRANCH}"
git add contracts/schemas/vitals/vitals.schema.json contracts/contract-pin.json

if git diff --cached --quiet; then
  echo "No staged changes to commit."
  exit 0
fi

git commit -m "chore: vendor telemetry contract ${TAG}" \
  -m "- Updated contracts/schemas/vitals/vitals.schema.json" \
  -m "- Updated contracts/contract-pin.json" \
  -m "- Pinned to ${TAG} (${COMMIT})"

git push --force-with-lease origin "${BRANCH}"

PR_NUMBER=$(gh pr list --head "${BRANCH}" --base main --state open --json number --jq '.[0].number // empty')

PR_BODY=$(cat <<EOF
## Automated contract update

Vendored schema from [chaithubk/medtech-telemetry-contract@${TAG}](https://github.com/chaithubk/medtech-telemetry-contract/releases/tag/${TAG}).

| Field | Value |
|---|---|
| Tag | ${TAG} |
| Commit SHA | ${COMMIT} |
| Compatibility classification | ${CLASSIFICATION} |

### Review checklist
- [ ] Review schema diff in contracts/schemas/vitals/vitals.schema.json
- [ ] Review metadata diff in contracts/contract-pin.json
- [ ] Verify CI passes
- [ ] For breaking changes, complete migration checklist before merge
EOF
)

if [[ -n "${PR_NUMBER}" ]]; then
  gh pr edit "${PR_NUMBER}" --title "chore: vendor telemetry contract ${TAG}" --body "${PR_BODY}"
else
  gh pr create --title "chore: vendor telemetry contract ${TAG}" --body "${PR_BODY}" --base main --head "${BRANCH}"
fi
