// Copyright 2017 Ground Breaking Games. All Rights Reserved.

#pragma once

#define CONDITIONAL_RETURN(Expression) if (!(Expression)) { return; }
#define CONDITIONAL_RETURN_VALUE(Expression, Value) if (!(Expression)) { return (Value); }