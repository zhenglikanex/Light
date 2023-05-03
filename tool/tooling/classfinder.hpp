/* ========================================================================= */
/* Copyright (C) 2017-2018 Arvid Gerstmann. All rights reserved.             */
/* ========================================================================= */

#ifndef METAREFLECT_CLASSFINDER_HPP
#define METAREFLECT_CLASSFINDER_HPP
#pragma once

#include "utils.hpp"
#include "reflectedclass.hpp"
#include <set>
#include <iostream>

class ClassFinder : public MatchFinder::MatchCallback {
public:
  virtual void run(MatchFinder::MatchResult const &result) override {
    m_context = result.Context;
    m_sourceman = result.SourceManager;

    CXXRecordDecl const *record =
        result.Nodes.getNodeAs<clang::CXXRecordDecl>("id");

    // 只处理分析中的文件
    if (record) {
      auto fileName = m_sourceman->getFilename(record->getLocation());
      if (fileName.find(':') != StringRef::npos) {
        return FoundRecord(record);
      }
    }

    EnumDecl const *enumType = result.Nodes.getNodeAs<clang::EnumDecl>("id");
    if (enumType) {
      auto fileName = m_sourceman->getFilename(enumType->getLocation());
      if (fileName.find(':') != StringRef::npos) {
        m_startEnum = true;
        SmallString<64> enumName;
        raw_svector_ostream stos(enumName);
        enumType->printQualifiedName(stos);
        m_enums.emplace_back(std::string(enumName));
        return;
      }
    }

    EnumConstantDecl const *enumValue =
        result.Nodes.getNodeAs<clang::EnumConstantDecl>("id");
    if (enumValue && m_startEnum) {
      m_enums.back().AddValue(enumValue->getNameAsString());
      return;
    }
    m_startEnum = false;

    FieldDecl const *field = result.Nodes.getNodeAs<clang::FieldDecl>("id");
    if (field) {
      return FoundField(field);
    }

    CXXMethodDecl const *function =
        result.Nodes.getNodeAs<clang::CXXMethodDecl>("id");
    if (function) {
      return FoundFunction(function);
    }
    // return FoundFunction(function);
  }

  void GenReflectionGenerated() {
    std::error_code ec;
    
    raw_fd_ostream os("C:/Project/Light/engine/reflection_generated/reflection_generated.h", ec);
    assert(!ec && "error opening file");

    os << "#pragma once \n";
    os << "namespace meta_generated {\n";
    for (auto& file : m_allFiles) {
      os << "extern void Register" << file << "();\n";
    }
    

    os << "static void RegisterTypesGeneratedImpl() { \n";
    for (auto &file : m_allFiles) {
      os << "Register" << file << "();\n";
    }
    os << "}\n";
    os << "}";

    raw_fd_ostream os2(
        "C:/Project/Light/engine/reflection_generated/reflection_generated.cpp",
        ec);

    os2 << "#include \"reflection_generated.h\"\n";
    os2 << "void RegisterTypesGenerated() { \n";
    os2 << "meta_generated::RegisterTypesGeneratedImpl();\n";
    os2 << "}\n";

  }

  virtual void onStartOfTranslationUnit() override {}

  virtual void onEndOfTranslationUnit() override {
    if (m_fileName.empty()) {
      return;
    }

    std::string genName = m_fileName;
    genName = genName.substr(genName.rfind('\\') + 1);
    genName.erase(genName.rfind('.'));
    m_allFiles.push_back(genName);
    genName.append(".generated.cpp");

    /* For stdout output. */
    /* raw_fd_ostream os(1, false); */
    std::error_code ec;
    raw_fd_ostream os("engine/reflection_generated/" + genName, ec);
    assert(!ec && "error opening file");
    /* File header: */
    os << "/* this file is auto-generated. do not edit! */\n";
    os << "#include \"engine/reflection/registry.h\"\n";
    os << "#include \"reflection_generated.h\"\n";
    os << "#include" << '"' << m_fileName << '"' << "\n";
    os << "using namespace light;\n";
    os << "using namespace meta;\n";
    os << "namespace meta_generated { \n"
       << "static void AutoRegisterTypes() { \n";
    for (auto enumType : m_enums) {
      enumType.Generate(m_context, os);
    }
    for (auto &[name, cls] : m_classes) {
      cls.Generate(m_context, os);
    }
    os << "};\n"
       << "void Register" << m_allFiles.back() << "() {\n"
       << "AutoRegisterTypes();\n"
       << "}\n"
       << "}";

    m_enums.clear();
    m_classes.clear();
    m_fileName.clear();
  }

protected:
  void FoundRecord(CXXRecordDecl const *record) {
    m_fileName = m_sourceman->getFilename(record->getLocation());
    
    m_classes.emplace(record->getNameAsString(), record);
  }

  void FoundField(FieldDecl const *field) {
    if (field->getParent()) {
      auto parent = dyn_cast<clang::CXXRecordDecl>(field->getParent());
      

      if (parent) {
        m_allClassField[parent].emplace_back(field);

        auto it = m_classes.find(parent->getNameAsString());
        if (it != m_classes.end()) {
          for (auto &base : parent->bases()) {
            for (auto &base_field : m_allClassField[base.getType()->getAsCXXRecordDecl()]) {
              it->second.AddField(base_field);
            }
            m_allClassField[base.getType()->getAsCXXRecordDecl()].clear();
          }
          
          it->second.AddField(field);
        }
      }
    }
  }

  void FoundFunction(CXXMethodDecl const *function) {
    if (function->getParent()) {
      auto parent = dyn_cast<clang::CXXRecordDecl>(function->getParent());
      if (parent &&
          function->getNameAsString() !=
              parent->getNameAsString() && function->getNameAsString().find(
                  '~') == StringRef::npos) {
        m_allClassMethod[parent].emplace_back(function);

        auto it = m_classes.find(parent->getNameAsString());
        if (it != m_classes.end()) {
          for (auto &base : parent->bases()) {
            for (auto &base_func :
                 m_allClassMethod[base.getType()->getAsCXXRecordDecl()]) {
              it->second.AddFunction(base_func);
            }
            m_allClassMethod[base.getType()->getAsCXXRecordDecl()].clear();
          }
          it->second.AddFunction(function);
        }
      }
    }
  }

protected:
  ASTContext *m_context;
  SourceManager *m_sourceman;
  std::set<std::string> m_headerFiles;
  std::unordered_map<std::string, ReflectedClass> m_classes;
  std::unordered_map<const clang::CXXRecordDecl*, std::vector<const clang::FieldDecl *>>
      m_allClassField;
  std::unordered_map<const clang::CXXRecordDecl *,
                     std::vector<const clang::CXXMethodDecl *>>
      m_allClassMethod;
  std::string m_fileName;
  std::vector<ReflectedEnum> m_enums;
  bool m_startEnum = false;
  std::vector<std::string> m_allFiles;
};

#endif /* METAREFLECT_CLASSFINDER_HPP */

