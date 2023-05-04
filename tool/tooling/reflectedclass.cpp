/* ========================================================================= */
/* Copyright (C) 2017-2018 Arvid Gerstmann. All rights reserved.             */
/* ========================================================================= */

#include "utils.hpp"
#include "reflectedclass.hpp"
#include "fieldgenerator.hpp"
#include "functiongenerator.hpp"

#include <iostream>
#include <format>
/* ========================================================================= */
/* Public API                                                                */
/* ========================================================================= */
void ReflectedClass::Generate(ASTContext *ctx, raw_ostream &os) {
  SmallString<64> str;
  SmallString<64> type;
  raw_svector_ostream stos(type);
  
  m_record->printQualifiedName(stos);

  SmallVector<PropertyAnnotations, 8> propertyAnnotations;
  SmallVector<FunctionAnnotations, 8> functionAnnotations;

  /* Verify CLASS annotation. */
  for (auto &attr : m_record->attrs()) {
    if (attr->getKind() == attr::Annotate) {
      auto ref = GetAnnotations(attr, str);
      assert(ref.startswith("reflect-class") && "incorrect annotation");
    }
  }

  os << "{\n"
     << std::format("TypeData &data = Registry::Get().AddTypeData<{}>(\"{}\");\n",
                    std::string(type),std::string( type));

  SmallString<64> baseType;
  raw_svector_ostream baseOs(baseType);
  for (auto& base : m_record->bases())
  {
    base.getType().getTypePtr()->getAsCXXRecordDecl()->printQualifiedName(
        baseOs);
    os << std::format("data.AddBaseType(\"{}\");\n", std::string(baseType));
  }

  /* Generate PROPERTY annotations. */
  FieldGenerator fieldGenerator(ctx, type);
  for (auto &field : m_fields) {
    Attr *attr = GetAnnotateAttr(field);
    StringRef attrStr;  
    if (attr) {
      attrStr = GetAnnotations(attr, str);
    }
    fieldGenerator.Generate(field, attrStr, os);
  }

  /* Generate FUNCTION annotations. */
  FunctionGenerator funcGenerator(ctx, type, "self->functions");
  for (auto &func : m_functions) {
    Attr *attr = GetAnnotateAttr(func);
    funcGenerator.Generate(func,os);
  }
  os << "}\n";
}

/* ========================================================================= */
/* Generator                                                                 */
/* ========================================================================= */
PropertyAnnotations
ReflectedClass::GenerateFieldAttributes(StringRef const &attr)
{
    /*assert(attr.startswith("reflect-property") && "incorrect annotation");
    auto pair = attr.split(";");

    PropertyAnnotations ret;
    auto setAttributes = [&ret](StringRef const &s) -> bool
    {
       
    };

    ForEachProperty(pair.second, setAttributes);
    return ret;*/
  return {};
}

FunctionAnnotations
ReflectedClass::GenerateFunctionAttributes(StringRef const &attr)
{
    assert(attr.startswith("reflect-function") && "incorrect annotation");
    auto pair = attr.split(";");

    FunctionAnnotations ret;
    auto setAttributes = [&ret](StringRef const &s) -> bool
    {
        if (s.equals("replicated")) {
            return (ret.replicated = true);
        }

        return false;
    };

    ForEachProperty(pair.second, setAttributes);
    return ret;
}

