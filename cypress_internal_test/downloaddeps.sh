#!/bin/sh
#
# downloaddeps.sh
# ===============
# Retrieves build dependencies for a project from a central server and
# extracts them into a local directory.
#
# Usage: downloaddeps.sh dependency_list.txt
#
#
# Configuration
# -------------
# The CY_DEP_DIR environment variable CY_DEP_DIR specifies the directory
# where the dependency files are stored. The default path is ~/.cydep
#
# The CY_DEP_URL environment variable specifies the URL to download
# dependencies from.
#
#
# Dependency list format
# ----------------------
# Each line of the dependency list file should contain the SHA256 hash of the
# file followed by a space and the relative path of the file on the server.
# The package type may be .zip, .tgz, .tar.gz, .tbz, .tar.bz2, or .tar.xz.
# Example line:
# 1d5f4f0416ff145b00bfba8ec989c3c49bdc9cc8f4faef8964cc193f06322029 thrift/thrift-0.11.0-msvc141-x86_64-mswin.tar.bz2
#
# This package would create a directory named:
# $CY_DEP_DIR/thrift-0.11.0-msvc141-x86_64-mswin
#
# The name may contain relative path components which affect the URI but not
# the location of the extracted files.
#
#
# Handling multiple platforms or configurations
# ---------------------------------------------
# The build script may select one of several dependency list files depending on
# criteria such as the platform or toolchain.
#
#
# Required tools
# --------------
# * bzip2
# * curl
# * dos2unix
# * gzip
# * tar
# * unzip
# * xz
#
# this must be the first non-commented line in the script. It ensures bash
# doesn't choke on \r on Windows.
(set -o igncr) 2>/dev/null && set -o igncr; # this comment is needed

set -$-ue${DEBUG+xv}
set +e

if [ -x /usr/bin/shasum ]; then
  SHA256SUM="shasum -a 256" # OS X doesn't have GNU coreutils
else
  SHA256SUM=sha256sum
fi
# OS X doesn't have dos2unix
DOS2UNIX=dos2unix
dos2unix --version >/dev/null 2>&1 || DOS2UNIX=cat

set -e

extract_file()
{
  archive_name="$1"
  archive_subdir="$2"
  opt_createdir="$3"
  archive_path="../_cache/$1"
  rm -rf "$TMP_DIR/$archive_subdir"
  extract_dir="$TMP_DIR"
  if [ "$opt_createdir" -ne 0 ]; then
    # The archive doesn't contain its own subdirectory. Create one based on the archive name.
    extract_dir="$TMP_DIR/$archive_subdir"
    archive_path="../$archive_path"
    mkdir -p "$extract_dir"
  fi
  case "$archive_path" in
    *.tar.gz|*.tar.bz2|*.tar.xz|*.tgz|*.tbz)
      echo "Extracting $archive_name..."
      ( cd "$extract_dir" && tar -xf "$archive_path" )
      mv "$TMP_DIR/$archive_subdir" "$CY_DEP_DIR/"
      ;;
    *.zip)
      echo "Extracting $archive_name..."
      ( cd "$extract_dir" && unzip -q "$archive_path" )
      mv "$TMP_DIR/$archive_subdir" "$CY_DEP_DIR/"
      ;;
    *)
      echo "$archive_name: Unsupported archive type"
      exit 1
      ;;
  esac
}

download_file()
{
  file_relpath="$1"
  file_name=$(basename "$file_relpath")
  tmp_path="$TMP_DIR/$file_name"
  case "$file_relpath" in
    http://* | https://*)
      # Absolute path to third-party HTTP resource
      uri="$file_relpath"
      ;;
    *)
      # Relative path to resource mirrored at CY_DEP_URL
      uri="$CY_DEP_URL/$file_relpath"
      ;;
  esac
  echo "Downloading $uri"
  rm -f "$tmp_path"
  curl -f --connect-timeout 10 "-#" -o "$tmp_path" "$uri"
}

verify_file()
{
  file_name="$1"
  expected_hash="$2"
  file_path="$TMP_DIR/$file_name"
  actual_hash="$($SHA256SUM "$file_path" | cut -c 1-64)"
  if [ "x$expected_hash" != "x$actual_hash" ]; then
    echo "$file_name: SHA256 mismatch:"
    echo "expected \"$expected_hash\""
    echo "but got  \"$actual_hash\""
    rm -f "$file_path"
    exit 1
  else
    mv "$file_path" "$CACHE_DIR"
  fi
}


if [ "${CY_DEP_DIR:-x}" != "x" ]; then
  true
elif [ "x$LOCALAPPDATA" != "x" ]; then
  CY_DEP_DIR="$LOCALAPPDATA/cydep" # Default location for Windows
elif [ "x$XDG_CACHE_HOME" != "x" ]; then
  CY_DEP_DIR="$XDG_CACHE_HOME/cydep" # Default location for Linux/OSX
elif [ "x$HOME" != "x" ]; then
  CY_DEP_DIR="$HOME/.cache/cydep"  # Fallback location for Linux/OSX
else
  echo "CY_DEP_DIR environment variable must be set"
  exit 1
fi

CY_DEP_DIR=$(echo $CY_DEP_DIR | sed -e 's/\\/\//g')
echo "Dependencies are in $CY_DEP_DIR"

if [ "${CY_DEP_URL:-x}" = "x" ]; then
  CY_DEP_URL="http://lassen.aus.cypress.com/~repoman"
fi

CACHE_DIR="$CY_DEP_DIR/_cache"        # Stores already downloaded files
TMP_DIR="$CY_DEP_DIR/_tmp"  # Stores partially downloaded files

if [ $# -ne 1 ]; then
  echo "Usage: downloaddeps.sh deps_file"
  exit 1
fi

# Create directories if they don't exist
if [ ! -d "$CACHE_DIR" ]; then
  mkdir -p "$CACHE_DIR"
fi
rm -rf "$TMP_DIR"
mkdir -p "$TMP_DIR"

deps_file="$1"
$DOS2UNIX < "$deps_file" | while read line; do
  arg_relpath="$(echo $line | awk '{ print $2; }')"
  arg_hash="$(echo $line | awk '{ print tolower($1); }')"
  # Read options. Options are separated by spaces and appear after the path.
  opt_createdir=0
  arg_options="$(echo $line | awk '{ for (i = 3; i <= NF; i++) { print $i; } }')"
  for opt in $arg_options; do
    case "$opt" in
      # createdir: Create a subdirectory based on the archive basename.
      #            This is for archives that do not contain their own subdirectory.
      createdir) opt_createdir=1;;
      *) echo "Warning: unsupported option for $arg_relpath";;
    esac
  done
  file_only=$(basename "$arg_relpath")
  # Remove extension
  dir_name=${file_only%.zip}
  dir_name=${dir_name%.tgz}
  dir_name=${dir_name%.tbz}
  dir_name=${dir_name%.gz}
  dir_name=${dir_name%.bz2}
  dir_name=${dir_name%.xz}
  dir_name=${dir_name%.tar}
  if [ -e "$CY_DEP_DIR/$dir_name" ]; then
    echo "$dir_name is present"
  else
    if [ ! -e "$CACHE_DIR/$file_only" ]; then
      download_file "$arg_relpath"
      verify_file "$file_only" "$arg_hash"
    fi
    extract_file "$file_only" "$dir_name" "$opt_createdir"
  fi
done
