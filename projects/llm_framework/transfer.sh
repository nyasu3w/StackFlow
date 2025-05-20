#!/bin/bash

#
# File transfer script created by Grok3
#   modified a little by nyasu3w2022
#
#  ssh needs the password every time.
#  public key authentication is recommended.


# リモートマシンのホスト名またはIPアドレス
REMOTE_HOST="m5stack-llm.local"
# リモートマシンのユーザー名
REMOTE_USER="root"
# ローカルのdistディレクトリ
SOURCE_DIR="dist"
# リモートでの検索対象ディレクトリ
REMOTE_SEARCH_DIR="/opt/m5stack"

# distディレクトリ内のファイルを処理
for file in "$SOURCE_DIR"/*; do
    if [ -f "$file" ]; then
        # ファイル名を取得
        filename=$(basename "$file")
        echo "処理中: $filename"

        # リモートで同名ファイルの場所を検索
        # findコマンドで/opt以下を探索し、ファイル名が一致するものを探す
        remote_path=$(ssh "${REMOTE_USER}@${REMOTE_HOST}" "find ${REMOTE_SEARCH_DIR} -type f -name '${filename}' -exec dirname {} \; | head -n 1")

        if [ -n "$remote_path" ]; then
            # 見つかった場合、そのディレクトリに転送
            echo "転送先: ${remote_path}"
            scp "$file" "${REMOTE_USER}@${REMOTE_HOST}:${remote_path}/"
            if [ $? -eq 0 ]; then
                echo "転送成功: $filename -> ${remote_path}/"
            else
                echo "転送失敗: $filename"
            fi
        else
            # 見つからなかった場合
            echo "警告: $filename はリモートの ${REMOTE_SEARCH_DIR} 以下に見つかりませんでした。転送をスキップします。"
        fi
    fi
done

echo "転送処理が完了しました。"
