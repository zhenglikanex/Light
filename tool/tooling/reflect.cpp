#include <stdio.h>

#include "annotations.hpp"
#include "classfinder.hpp"
#include "utils.hpp"


static llvm::cl::OptionCategory g_ToolCategory("metareflect options");

int
main(int argc, const char **argv)
{
    /* Parse command-line options. */


  auto Options =
        clang::tooling::CommonOptionsParser::create(argc, argv, g_ToolCategory);
    auto &db = Options->getCompilations();
  
    for (auto &cmd : db.getAllCompileCommands()) {
      printf("CommandLine:");
      for (auto &opt : cmd.CommandLine)
        printf(" %s", opt.c_str());
      printf("\n");
      printf("File: %s\n", cmd.Filename.c_str());
      printf("Directory: %s\n", cmd.Directory.c_str());
      printf("Output: %s\n", cmd.Output.c_str());
    }
  clang::tooling::ClangTool tool(Options->getCompilations(),
                                 Options->getSourcePathList());
  
  tool.appendArgumentsAdjuster(
      [](const CommandLineArguments &arguments, StringRef Filename) {
        CommandLineArguments new_arguments = arguments;
    auto it = std::ranges::find_if(new_arguments, [](const std::string &param) {
              return param.find("/D") != std::string::npos;
            });
        new_arguments.insert(it, "/DMETA_PARSER");
        return new_arguments;
      });
   for(auto& file :Options->getSourcePathList()) {
        printf("%s\n", file.c_str());
   }

    /* The classFinder class is invoked as callback. */
    ClassFinder classFinder;
    MatchFinder finder;

    /* Search for all records (class/struct) with an 'annotate' attribute. */
    static DeclarationMatcher const classMatcher =
        cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
    finder.addMatcher(classMatcher, &classFinder);

    static DeclarationMatcher const enumMatcher =
        enumDecl(decl().bind("id"), hasAttr(attr::Annotate));
    finder.addMatcher(enumMatcher, &classFinder);

    static DeclarationMatcher const enumValueMatcher =
        enumConstantDecl(decl().bind("id"));
    finder.addMatcher(enumValueMatcher, &classFinder);

    /* Search for all fields with an 'annotate' attribute. */
    static DeclarationMatcher const propertyMatcher =
        fieldDecl(decl().bind("id"));
    finder.addMatcher(propertyMatcher, &classFinder);

    /* Search for all functions with an 'annotate' attribute. */
    static DeclarationMatcher const functionMatcher =
        cxxMethodDecl(decl().bind("id"));
    finder.addMatcher(functionMatcher, &classFinder);

    int ret = tool.run(newFrontendActionFactory(&finder).get());

    classFinder.GenReflectionGenerated();
    return ret;
}

