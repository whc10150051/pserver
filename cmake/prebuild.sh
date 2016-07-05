#!/bin/sh

# Copyright (c) 2016 Encotes
# All rights reserved
#
# Исключительное право (c) 2016 принадлежит ООО Encotes
# Все права защищены

#   This script is used as a pre-build event for rdc
# It is called automatically by makefile as a dependency for rdc and
# before building of rdc is started. Nothing else should be configured
# for calling this script at Jenkins server or user PC.
#
#   The script creates or updates C++ header file that is passed as an argument
# with some build information: user name who has started the building process,
# date and number of a build, git branch name and hash of the last commit.
# If the script is started at Jenkins server it will pass to the script
# correct values for the build number, git branch name and last commit hash.
# Otherwise the script will try to retreive values by calling git commands and by
# incrementing number of the build (previous value will be parsed from the in/out file).
# An "Unknown" value will be populated for information fields that the script
# failed to retreive. The rest date and use name fields will be retreived by
# calling unix commands.
#
#   Currently, makefile passes as an argument "src/util/BuildInfo.h" file.
# The file can be absent or exist at disk. It is added to git ignore files
# since there is not need to store it at repository. The file is not added
# as source file into CMake scripts. However, there is no problem with
# including it at rdc source code and accessing constants values. Also, rdc
# compiles fine in that case.

# check arguments
if [ "$#" -ne 1 ] ; then
  echo "Usage: $0 <IN/OUT FILE>"
  exit 1
fi
if [ ! $1 ] ; then
  echo "File name cannot be empty"
  exit 2
fi

# get values with unix commands
RDC_DATE=$(date)
RDC_USER=$(whoami)

# get values from env. variables exported by Jenkins
RDC_NUMBER=$BUILD_NUMBER
RDC_BRANCH=$GIT_BRANCH
RDC_COMMIT=$GIT_COMMIT


# if env variables are empty build is started manually without jenkins
# get last build number from the file if it exists and increment by 1
# if the file doesn't exist the build number is one
if [ ! $RDC_NUMBER ] ; then
  value=$(awk '/#define/{ if ($2 == "RES_PRODUCT_BUILD_NUMBER"){ gsub(/\"/, "", $3); print $3 }}' <$1)
  let value++
  if [ $? -eq 0 ] ; then
    RDC_NUMBER=$value
  else
    RDC_NUMBER=1
  fi
fi

# get other values from git
if [ ! $RDC_BRANCH ] ; then
  value=$(git rev-parse --abbrev-ref HEAD)
  if [ $? -eq 0 ] ; then
    RDC_BRANCH=$value
  fi
fi

if [ ! $RDC_COMMIT ] ; then
  value=$(git rev-parse HEAD)
  if [ $? -eq 0 ] ; then
    RDC_COMMIT=$value
  fi
fi


# if variables are still empty fill them with default values
if [ ! $RDC_BRANCH ] ; then
  RDC_BRANCH="Unknown"
fi

if [ ! $RDC_COMMIT ] ; then
  RDC_COMMIT="Unknown"
fi

# populate file
cat > $1 <<EOF
/*
 * Copyright (c) 2016 Encotes
 * All rights reserved
 *
 * Исключительное право (c) 2015 принадлежит ООО Encotes
 * Все права защищены
 */

#pragma once

#define RES_PRODUCT_BUILD_DATE $RDC_DATE
#define RES_PRODUCT_BUILD_NUMBER $RDC_NUMBER
#define RES_PRODUCT_BUILD_USER $RDC_USER

#define RES_PRODUCT_GIT_BRANCH $RDC_BRANCH
#define RES_PRODUCT_GIT_COMMIT $RDC_COMMIT
EOF

# print file name and content into the build log
echo $1
cat $1
