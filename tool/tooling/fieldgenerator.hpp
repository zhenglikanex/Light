/* ========================================================================= */
/* Copyright (C) 2017-2018 Arvid Gerstmann. All rights reserved.             */
/* ========================================================================= */

#ifndef METAREFLECT_FIELD_GENERATOR_HPP
#define METAREFLECT_FIELD_GENERATOR_HPP
#pragma once

#include "utils.hpp"
#include "annotations.hpp"


inline raw_ostream &
operator<<(raw_ostream &os, PropertyAnnotations &p)
{
    return os;
}


/* ========================================================================= */
/* Field Generator                                                           */
/* ========================================================================= */
struct FieldGenerator
{
    ASTContext * const ctx;
    SmallString<64> fieldName;
    SmallString<64> typeName;
    raw_svector_ostream fieldNameOs{fieldName};
    raw_svector_ostream TypeNameOs{typeName};
    StringRef const type;

    explicit
    FieldGenerator(
        ASTContext *context,
        StringRef const &parentType)
        : ctx(context)
        , type(parentType)
    {}

    void
    Generate(
        FieldDecl const *field, StringRef attr,
        raw_ostream &os)
    {
        fieldName.clear();
        typeName.clear();

        field->printName(fieldNameOs);
        GetRealTypeName(ctx, field->getType(), TypeNameOs);

        if(attr.size() > 0) {
          os << std::format("data.AddField<{}, {}>(\"{}\", &{}::{},{});\n", std::string(type),
                      std::string(typeName), std::string(fieldName),std::string(type), std::string(fieldName),std::string(attr));
        } else {
          os << std::format("data.AddField<{}, {}>(\"{}\", &{}::{});\n",
                            std::string(type), std::string(typeName),
                            std::string(fieldName), std::string(type),
                            std::string(fieldName));
        }
    }
};

#endif /* METAREFLECT_FIELD_GENERATOR_HPP */
