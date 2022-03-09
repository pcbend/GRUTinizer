#!/bin/bash

script_dir=$(dirname $(readlink -f "$0"))
include_file="$script_dir"/../include/GVersion.h

release_commit=$(git describe --abbrev=0 --match="v*")
release_num=$(echo "$release_commit" | sed -e 's/v//')
release_time=$(git show -s --format=%ai "$release_commit" | tail -n 1)
release_name=$(git rev-parse "$release_commit" | xargs git cat-file -p | tail -n1)

git_commit=$(git describe)
git_branch=$(git branch | sed -n '/\* /s///p')
git_commit_time=$(git show -s --format=%ai "$git_commit" | tail -n 1)

read -r -d '' file_contents <<EOF
#ifndef GRUT_GVERSION
#define GRUT_GVERSION

#define GRUT_RELEASE "$release_num"
#define GRUT_RELEASE_TIME "$release_time"
#define GRUT_RELEASE_NAME "$release_name"

#define GRUT_GIT_COMMIT "$git_commit"
#define GRUT_GIT_BRANCH "$git_branch"
#define GRUT_GIT_COMMIT_TIME "$git_commit_time"

#endif /* GRUT_GVERSION */
EOF

echo "$file_contents" > "$include_file"
