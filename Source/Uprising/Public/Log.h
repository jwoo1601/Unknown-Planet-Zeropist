// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#include "LogMacros.h"

#define LOG(Category, Text, ...) UE_LOG(Category, Display, TEXT(Text), __VA_ARGS__)
#define LOG_ONLY(Category, Text, ...) UE_LOG(Category, Log, TEXT(Text), __VA_ARGS__)
#define WARNING(Category, Text, ...) UE_LOG(Category, Warning, TEXT(Text), __VA_ARGS__)
#define ERROR(Category, Text, ...) UE_LOG(Category, Error, TEXT(Text), __VA_ARGS__)
#define FATAL(Category, Text, ...) UE_LOG(Category, Fatal, TEXT(Text), __VA_ARGS__)