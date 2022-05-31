#!/bin/sh

force=0
current_dir=${PWD}
sdk_path=$current_dir/../..

check_file_presence() {
    if [ $2 -eq 0 ]; then
        echo "$1", please remove it before install or run with -f
        return 1
    fi
    echo Removing already "$1"

    rm -f "$1"
    return 0
}

if [ "$1" = "-f" ]; then
    force=1
fi

check_file_presence $sdk_path/.git/hooks/pre-commit $force
if [ ! "$?" -eq 0 ]; then
    exit $?
fi
check_file_presence $sdk_path/.git/hooks/pre-push $force
if [ ! "$?" -eq 0 ]; then
    exit $?
fi

#cp pre-commit $sdk_path/.git/hooks/pre-commit
#cp pre-push $sdk_path/.git/hooks/pre-push
ln -s $sdk_path/scripts/git-hooks/pre-commit $sdk_path/.git/hooks/pre-commit
ln -s $sdk_path/scripts/git-hooks/pre-push $sdk_path/.git/hooks/pre-push
echo symbol link has been created for hooks

exit 0
