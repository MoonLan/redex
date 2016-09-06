/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fstream>
#include <iostream>
#include <memory>

#include "ProguardLexer.h"
#include "ProguardMap.h"
#include "ProguardParser.h"

#include <algorithm>

namespace redex {
namespace proguard_parser {

bool parse_boolean_command(std::vector<unique_ptr<Token>>::iterator* it,
                           token boolean_option,
                           bool* option,
                           bool value) {
  if ((**it)->type != boolean_option) {
    return false;
  }
  (*it)++;
  *option = value;
  return true;
}

void skip_to_next_command(std::vector<unique_ptr<Token>>::iterator* it) {
  while (((**it)->type != token::eof_token) && (!(**it)->is_command())) {
    (*it)++;
  }
}

bool parse_single_filepath_command(std::vector<unique_ptr<Token>>::iterator* it,
                                   token filepath_command_token,
                                   std::string* filepath) {
  if ((**it)->type == filepath_command_token) {
    unsigned int line_number = (**it)->line;
    (*it)++; // Consume the command token.
    // Fail without consumption if this is an end of file token.
    if ((**it)->type == token::eof_token) {
      cerr << "Expecting at least one file as an argument but found end of "
              "file at line "
           << line_number << endl;
      return true;
    }
    // Fail without consumption if this is a command token.
    if ((**it)->is_command()) {
      cerr << "Expecting a file path argument but got command "
           << (**it)->show() << " at line  " << (**it)->line << endl;
      return true;
    }
    // Parse the filename.
    if ((**it)->type != token::filepath) {
      cerr << "Expected a filepath but got " << (**it)->show() << " at line "
           << (**it)->line << endl;
      return true;
    }
    *filepath = static_cast<Filepath*>((*it)->get())->path;
    (*it)++; // Consume the filepath token
    return true;
  }
  return false;
}

std::vector<std::string> parse_filepaths(
    std::vector<unique_ptr<Token>>::iterator* it) {
  std::vector<std::string> filepaths;
  if ((**it)->type != token::filepath) {
    cerr << "Expected filepath but got " << (**it)->show() << " at line "
         << (**it)->line << endl;
    return filepaths;
  }
  while ((**it)->type == token::filepath) {
    filepaths.push_back(static_cast<Filepath*>((*it)->get())->path);
    (*it)++;
  }
  return filepaths;
}

bool parse_filepath_command(std::vector<unique_ptr<Token>>::iterator* it,
                            token filepath_command_token,
                            std::vector<std::string>* filepaths) {
  if ((**it)->type == filepath_command_token) {
    unsigned int line_number = (**it)->line;
    (*it)++; // Consume the command token.
    // Fail without consumption if this is an end of file token.
    if ((**it)->type == token::eof_token) {
      cerr << "Expecting at least one file as an argument but found end of "
              "file at line "
           << line_number << endl;
      return true;
    }
    // Fail without consumption if this is a command token.
    if ((**it)->is_command()) {
      cerr << "Expecting a file path argument but got command "
           << (**it)->show() << " at line  " << (**it)->line << endl;
      return true;
    }
    // Parse the filename.
    if ((**it)->type != token::filepath) {
      cerr << "Expected a filepath but got " << (**it)->show() << " at line "
           << (**it)->line << endl;
      return true;
    }
    for (const auto& filepath : parse_filepaths(it)) {
      filepaths->push_back(filepath);
    }
    return true;
  }
  return false;
}

bool parse_optional_filepath_command(
    std::vector<unique_ptr<Token>>::iterator* it,
    token filepath_command_token,
    std::vector<std::string>* filepaths) {
  if ((**it)->type != filepath_command_token) {
    return false;
  }
  (*it)++; // Consume the command token.
  // Parse an optional filepath argument.
  if ((**it)->type == token::filepath) {
    filepaths->push_back(static_cast<Filepath*>((*it)->get())->path);
    (*it)++;
  }
  return true;
}

bool parse_jars(std::vector<unique_ptr<Token>>::iterator* it,
                token jar_token,
                std::vector<std::string>* jars) {
  if ((**it)->type == jar_token) {
    unsigned int line_number = (**it)->line;
    (*it)++; // Consume the jar token.
    // Fail without consumption if this is an end of file token.
    if ((**it)->type == token::eof_token) {
      cerr << "Expecting at least one file as an argument but found end of "
              "file at line "
           << line_number << endl;
      return true;
    }
    // Parse the list of filenames.
    for (const auto& filepath : parse_filepaths(it)) {
      jars->push_back(filepath);
    }
    return true;
  }
  return false;
}

bool parse_dontusemixedcaseclassnames(
    std::vector<unique_ptr<Token>>::iterator* it,
    bool* dontusemixedcaseclassnames) {
  if ((**it)->type != token::dontusemixedcaseclassnames_token) {
    return false;
  }
  *dontusemixedcaseclassnames = true;
  (*it)++;
  return true;
}

bool parse_dontpreverify(std::vector<unique_ptr<Token>>::iterator* it,
                         bool* dontpreverify) {
  if ((**it)->type != token::dontpreverify_token) {
    return false;
  }
  *dontpreverify = true;
  (*it)++;
  return true;
}

bool parse_verbose(std::vector<unique_ptr<Token>>::iterator* it,
                   bool* verbose) {
  if ((**it)->type != token::verbose_token) {
    return false;
  }
  *verbose = true;
  (*it)++;
  return true;
}

bool parse_bool_command(std::vector<unique_ptr<Token>>::iterator* it,
                        token bool_command_token,
                        bool new_value,
                        bool* bool_value) {
  if ((**it)->type == bool_command_token) {
    (*it)++; // Consume the boolean command token.
    *bool_value = new_value;
    return true;
  }
  return false;
}

bool parse_repackageclasses(std::vector<unique_ptr<Token>>::iterator* it) {
  if ((**it)->type != token::repackageclasses) {
    return false;
  }
  // Ignore repackageclasses.
  (*it)++;
  if ((**it)->type == token::identifier) {
    cerr << "Ignoring -repackageclasses "
         << static_cast<Identifier*>((*it)->get())->ident << endl;
    (*it)++;
  }
  return true;
}

bool parse_target(std::vector<unique_ptr<Token>>::iterator* it,
                  std::string* target_version) {
  if ((**it)->type == token::target) {
    (*it)++; // Consume the target command token.
    // Check to make sure the next token is a version token.
    if ((**it)->type != token::target_version_token) {
      cerr << "Expected a target version but got " << (**it)->show()
           << " at line " << (**it)->line << endl;
      return true;
    }
    *target_version = static_cast<TargetVersion*>((*it)->get())->target_version;
    // Consume the target version token.
    (*it)++;
    return true;
  }
  return false;
}

bool parse_allowaccessmodification(std::vector<unique_ptr<Token>>::iterator* it,
                                   bool* allowaccessmodification) {
  if ((**it)->type != token::allowaccessmodification_token) {
    return false;
  }
  (*it)++;
  *allowaccessmodification = true;
  return true;
}

bool parse_filter_list_command(std::vector<unique_ptr<Token>>::iterator* it,
                               token filter_command_token,
                               std::vector<std::string>* filters) {
  if ((**it)->type != filter_command_token) {
    return false;
  }
  (*it)++;
  while ((**it)->type == token::filter_pattern) {
    filters->push_back(static_cast<Filter*>((*it)->get())->filter);
    (*it)++;
  }
  return true;
}

bool is_modifier(token tok) {
  return tok == token::includedescriptorclasses_token ||
         tok == token::allowshrinking_token ||
         tok == token::allowoptimization_token ||
         tok == token::allowobfuscation_token;
}

bool parse_modifiers(std::vector<unique_ptr<Token>>::iterator* it,
                     KeepSpec* keep) {
  while ((**it)->type == token::comma) {
    (*it)++;
    if (!is_modifier((**it)->type)) {
      cerr << "Expected keep option modifier but found : " << (**it)->show()
           << " at line number " << (**it)->line << endl;
      return false;
    }
    switch ((**it)->type) {
    case token::includedescriptorclasses_token:
      keep->includedescriptorclasses = true;
      break;
    case token::allowshrinking_token:
      keep->allowshrinking = true;
      break;
    case token::allowoptimization_token:
      keep->allowoptimization = true;
      break;
    case token::allowobfuscation_token:
      keep->allowobfuscation = true;
      break;
    default:
      break;
    }
    (*it)++;
  }
  return true;
}

bool parse_classname(std::vector<unique_ptr<Token>>::iterator* it,
                     KeepSpec* keep) {
  return true;
}

AccessFlag process_access_modifier(token type, bool* is_access_flag) {
  *is_access_flag = true;
  switch (type) {
  case token::publicToken:
    return AccessFlag::PUBLIC;
  case token::privateToken:
    return AccessFlag::PRIVATE;
  case token::final:
    return AccessFlag::FINAL;
  case token::interface:
    return AccessFlag::INTERFACE;
  case token::abstract:
    return AccessFlag::ABSTRACT;
  case token::synthetic:
    return AccessFlag::SYNTHETIC;
  case token::annotation:
    return AccessFlag::ANNOTATION;
  case token::enumToken:
    return AccessFlag::ENUM;
  case token::staticToken:
    return AccessFlag::STATIC;
  case token::native:
    return AccessFlag::NATIVE;
  case token::protectedToken:
    return AccessFlag::PROTECTED;
  case token::transient:
    return AccessFlag::TRANSIENT;
  default:
    *is_access_flag = false;
    return AccessFlag::PUBLIC;
  }
}

bool is_negation_or_class_access_modifier(token type) {
  switch (type) {
  case token::notToken:
  case token::publicToken:
  case token::privateToken:
  case token::protectedToken:
  case token::final:
  case token::abstract:
  case token::synthetic:
  case token::native:
  case token::staticToken:
  case token::transient:
    return true;
  default:
    return false;
  }
}

std::string parse_annotation_type(
    std::vector<unique_ptr<Token>>::iterator* it) {
  if ((**it)->type != token::annotation_application) {
    return "";
  }
  (*it)++;
  if ((**it)->type != token::identifier) {
    cerr << "Expecting a class identifier after @ but got " << (**it)->show()
         << " at line " << (**it)->line << endl;
    return "";
  }
  auto typ = static_cast<Identifier*>((*it)->get())->ident;
  (*it)++;
  return typ;
}

bool parse_access_flags(std::vector<unique_ptr<Token>>::iterator* it,
                        set<AccessFlag>* setFlags,
                        set<AccessFlag>* unsetFlags) {
  bool negated = false;
  while (is_negation_or_class_access_modifier((**it)->type)) {
    // Consume the negation token if present.
    if ((**it)->type == token::notToken) {
      negated = true;
      (*it)++;
      continue;
    }
    bool ok;
    AccessFlag access_flag = process_access_modifier((**it)->type, &ok);
    if (ok) {
      (*it)++;
      if (negated) {
        if (setFlags->find(access_flag) != setFlags->end()) {
          cerr << "Access flag " << (**it)->show()
               << " occurs with conflicting settings at line " << (**it)->line
               << endl;
          return false;
        }
        unsetFlags->emplace(access_flag);
        negated = false;
      } else {
        if (unsetFlags->find(access_flag) != unsetFlags->end()) {
          cerr << "Access flag " << (**it)->show()
               << " occurs with conflicting settings at line " << (**it)->line
               << endl;
          return false;
        }
        setFlags->emplace(access_flag);
        negated = false;
      }
    } else {
      break;
    }
  }
  return true;
}

void gobble_semicolon(std::vector<unique_ptr<Token>>::iterator* it, bool* ok) {
  *ok = true;
  if ((**it)->type != token::semiColon) {
    cerr << "Expecting a semicolon but found " << (**it)->show() << " at line "
         << (**it)->line << std::endl;
    *ok = false;
    return;
  }
  (*it)++;
}

void skip_to_semicolon(std::vector<unique_ptr<Token>>::iterator* it) {
  while (((**it)->type != token::semiColon) &&
         ((**it)->type != token::eof_token)) {
    (*it)++;
  }
  if ((**it)->type == token::semiColon) {
    (*it)++;
  }
}

void parse_member_specification(
    std::vector<unique_ptr<Token>>::iterator* it,
    ClassSpecification* class_spec,
    bool* ok) {
  MemberSpecification member_specification;
  *ok = true;
  member_specification.annotationType = parse_annotation_type(it);
  if (!parse_access_flags(it,
                          &member_specification.requiredSetAccessFlags,
                          &member_specification.requiredUnsetAccessFlags)) {
    // There was a problem parsing the access flags. Return an empty class spec
    // for now.
    cerr << "Problem parsing access flags for member specification.\n";
    *ok = false;
    skip_to_semicolon(it);
    return;
  }
  // Check for <methods>
  if ((**it)->type == token::methods) {
    member_specification.name = "";
    (*it)++;
    gobble_semicolon(it, ok);
    class_spec->methodSpecifications.push_back(member_specification);
    return;
  }
  // Check for <fields>
  if ((**it)->type == token::fields) {
    member_specification.name = "";
    (*it)++;
    gobble_semicolon(it, ok);
    class_spec->fieldSpecifications.push_back(member_specification);
    return;
  }
  // Check for <init>
  if ((**it)->type == token::init) {
    member_specification.name = "<init>";
    // Currently only support <init>()
    member_specification.descriptor = "()V";
    class_spec->methodSpecifications.push_back(member_specification);
    (*it)++;
    skip_to_semicolon(it);
    return;
  }
  // The next token better be an identifier.
  if ((**it)->type != token::identifier) {
    cerr << "Expecting field or member specification but got " << (**it)->show()
         << " at line " << (**it)->line << endl;
    *ok = false;
    skip_to_semicolon(it);
    return;
  }
  std::string ident = static_cast<Identifier*>((*it)->get())->ident;
  // Check for "*".
  if (ident == "*") {
    member_specification.name = "";
    (*it)++;
    gobble_semicolon(it, ok);
    class_spec->methodSpecifications.push_back(member_specification);
    class_spec->fieldSpecifications.push_back(member_specification);
    return;
  }
  // This token is the type for the member specification.
  if ((**it)->type != token::identifier) {
    cerr << "Expecting type identifier but got " << (**it)->show()
         << " at line " << (**it)->line << endl;
    *ok = false;
    skip_to_semicolon(it);
    return;
  }
  std::string typ = static_cast<Identifier*>((*it)->get())->ident;
  (*it)++;
  member_specification.descriptor = convert_type(typ);
  if ((**it)->type != token::identifier) {
    cerr << "Expecting identifier name for class member but got " << (**it)->show()
         << " at line " << (**it)->line << endl;
    *ok = false;
    skip_to_semicolon(it);
    return;
  }
  member_specification.name = static_cast<Identifier*>((*it)->get())->ident;
  (*it)++;
  // For the moment just skip past the argument specification for methods.
  skip_to_semicolon(it);
  if (member_specification.descriptor[0] == '(') {
    class_spec->methodSpecifications.push_back(member_specification);
  } else {
    class_spec->fieldSpecifications.push_back(member_specification);
  }
  return;
}

void parse_member_specifications(std::vector<unique_ptr<Token>>::iterator* it,
                                 ClassSpecification* class_spec,
                                 bool* ok) {
  if ((**it)->type == token::openCurlyBracket) {
    (*it)++;
    while (((**it)->type != token::closeCurlyBracket) &&
           ((**it)->type != token::eof_token)) {
      parse_member_specification(it, class_spec, ok);
      if (!*ok) {
        // We failed to parse a member specification so skip to the next
        // semicolon.
        skip_to_semicolon(it);
      }
    }
    if ((**it)->type == token::closeCurlyBracket) {
      (*it)++;
    }
  }
}

ClassSpecification parse_class_specification(
    std::vector<unique_ptr<Token>>::iterator* it, bool* ok) {
  ClassSpecification class_spec;
  *ok = true;
  class_spec.annotationType = parse_annotation_type(it);
  if (!parse_access_flags(
          it, &class_spec.setAccessFlags, &class_spec.unsetAccessFlags)) {
    // There was a problem parsing the access flags. Return an empty class spec
    // for now.
    cerr << "Problem parsing access flags for class specification.\n";
    *ok = false;
    return class_spec;
  }
  // According to the ProGuard grammer the next token could be a '!' to express
  // a rule that
  // says !class or !interface or !enum. We choose to not implement this
  // feature.
  if ((**it)->type == token::notToken) {
    cerr << "Keep rules that match the negation of class, interface or enum "
            "are not supported.\n";
    *ok = false;
    return class_spec;
  }
  // Make sure the next keyword is interface, class, enum or @interface.
  if (!(((**it)->type == token::interface) ||
        ((**it)->type == token::classToken) ||
        ((**it)->type == token::enumToken ||
         ((**it)->type == token::annotation)))) {
    cerr << "Expected interface, @interface, class or enum but got "
         << (**it)->show() << " at line number " << (**it)->line << endl;
    *ok = false;
    return class_spec;
  }
  (*it)++;
  // Parse the class name.
  if ((**it)->type != token::identifier) {
    cerr << "Expected class name but got " << (**it)->show() << " at line "
         << (**it)->line << endl;
    *ok = false;
    return class_spec;
  }
  class_spec.className = static_cast<Identifier*>((*it)->get())->ident;
  (*it)++;
  // Parse extends/implements if present, treating implements like extends.
  if (((**it)->type == token::extends) || ((**it)->type == token::implements)) {
    (*it)++;
    class_spec.extendsAnnotationType = parse_annotation_type(it);
    if ((**it)->type != token::identifier) {
      cerr << "Expecting a class name after extends/implements but got "
           << (**it)->show() << " at line " << (**it)->line << endl;
      *ok = false;
      class_spec.extendsClassName = "";
    } else {
      class_spec.extendsClassName =
          static_cast<Identifier*>((*it)->get())->ident;
    }
    (*it)++;
  }
  // Parse the member specifications, if there are any
  parse_member_specifications(it, &class_spec, ok);
  return class_spec;
}

bool parse_keep(std::vector<unique_ptr<Token>>::iterator* it,
                token keep_kind,
                std::vector<KeepSpec>* spec,
                bool* ok) {
  if ((**it)->type == keep_kind) {
    (*it)++; // Consume the keep token
    KeepSpec keep;
    if (!parse_modifiers(it, &keep)) {
      skip_to_next_command(it);
      return true;
    }
    keep.class_spec = parse_class_specification(it, ok);
    spec->push_back(keep);
    return true;
  }
  return false;
}

bool parse_class_specification_command(
    std::vector<unique_ptr<Token>>::iterator* it, token classspec_command) {
  if ((**it)->type != classspec_command) {
    return false;
  }
  (*it)++;
  // At the moment just ignore the class specification.
  skip_to_next_command(it);
  return true;
}

bool parse_keepnames(std::vector<unique_ptr<Token>>::iterator* it,
                     std::vector<KeepSpec>* keep_rules,
                     bool* ok) {
  if ((**it)->type != token::keepnames) {
    return false;
  }
  if (!parse_keep(it, token::keepnames, keep_rules, ok)) {
    cerr << "Failed to parse -keepnames rule at line " << (**it)->line << endl;
    return true;
  }
  // Set allowshrinking.
  keep_rules->back().allowshrinking = true;
  return true;
}

bool parse_keepclasssmembernames(std::vector<unique_ptr<Token>>::iterator* it,
                                 std::vector<KeepSpec>* keep_rules,
                                 bool* ok) {
  if ((**it)->type != token::keepclassmembernames) {
    return false;
  }
  if (!parse_keep(it, token::keepclassmembernames, keep_rules, ok)) {
    cerr << "Failed to parse -keepclasssmembernames rule\n";
    return true;
  }
  // Set allowshrinking.
  keep_rules->back().allowshrinking = true;
  return true;
}

bool parse_keepclasseswithmembernames(
    std::vector<unique_ptr<Token>>::iterator* it,
    std::vector<KeepSpec>* keep_rules,
    bool* ok) {
  if ((**it)->type != token::keepclasseswithmembernames) {
    return false;
  }
  if (!parse_keep(it, token::keepclasseswithmembers, keep_rules, ok)) {
    cerr << "Failed to parse -keepclasseswithmembernames rule\n";
    return true;
  }
  // Set allowshrinking.
  keep_rules->back().allowshrinking = true;
  return true;
}

void parse(std::vector<unique_ptr<Token>>::iterator it,
           std::vector<unique_ptr<Token>>::iterator tokens_end,
           ProguardConfiguration* pg_config,
           unsigned int* parse_errors) {
  *parse_errors = 0;
  bool ok;
  while (it != tokens_end) {
    // Break out if we are at the end of the token stream.
    if ((*it)->type == token::eof_token) {
      break;
    }
    if (!(*it)->is_command()) {
      cerr << "Expecting command but found " << (*it)->show() << " at line "
           << (*it)->line << endl;
      it++;
      skip_to_next_command(&it);
      continue;
    }

    // Input/Output Options
    if (parse_filepath_command(&it, token::include, &pg_config->includes))
      continue;
    if (parse_single_filepath_command(
            &it, token::basedirectory, &pg_config->basedirectory))
      continue;
    if (parse_jars(&it, token::injars, &pg_config->injars)) continue;
    if (parse_jars(&it, token::outjars, &pg_config->outjars)) continue;
    if (parse_jars(&it, token::libraryjars, &pg_config->libraryjars)) continue;
    // -skipnonpubliclibraryclasses not supported
    // -dontskipnonpubliclibraryclasses not supported
    // -dontskipnonpubliclibraryclassmembers not supported
    if (parse_filepath_command(
            &it, token::keepdirectories, &pg_config->keepdirectories))
      continue;
    if (parse_target(&it, &pg_config->target_version)) continue;
    // -forceprocessing not supported

    // Keep Options
    if (parse_keep(&it, token::keep, &pg_config->keep_rules, &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_keep(&it,
                   token::keepclassmembers,
                   &pg_config->keepclassmembers_rules,
                   &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_keep(&it,
                   token::keepclasseswithmembers,
                   &pg_config->keepclasseswithmembers_rules,
                   &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_keepnames(&it, &pg_config->keep_rules, &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_keepclasssmembernames(
            &it, &pg_config->keepclassmembers_rules, &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_keepclasseswithmembernames(
            &it, &pg_config->keepclasseswithmembers_rules, &ok)) {
      if (!ok) {
        (*parse_errors)++;
      }
      continue;
    }
    if (parse_optional_filepath_command(
            &it, token::printseeds, &pg_config->printseeds))
      continue;

    // Shrinking Options
    if (parse_bool_command(&it, token::dontshrink, false, &pg_config->shrink))
      continue;
    if (parse_optional_filepath_command(
            &it, token::printusage, &pg_config->printusage))
      continue;
    if (parse_class_specification_command(&it, token::whyareyoukeeping))
      continue;

    // Optimization Options
    if (parse_boolean_command(
            &it, token::dontoptimize, &pg_config->optimize, false))
      continue;
    if (parse_filter_list_command(
            &it, token::optimizations, &pg_config->optimization_filters))
      continue;
    // -optimizationpasses not supported
    if (parse_keep(&it,
                   token::assumenosideeffects,
                   &pg_config->assumesideeffects_rules,
                   &ok))
      continue;

    // Obfuscation Options
    if ((*it)->type == token::dontobfuscate) {
      pg_config->dontobfuscate = true;
      it++;
      continue;
    }
    if (parse_optional_filepath_command(
            &it, token::printmapping, &pg_config->printmapping))
      continue;
    if (parse_optional_filepath_command(
            &it, token::printconfiguration, &pg_config->printconfiguration))
      continue;

    if (parse_allowaccessmodification(&it, &pg_config->allowaccessmodification))
      continue;
    if (parse_dontusemixedcaseclassnames(
            &it, &pg_config->dontusemixedcaseclassnames))
      continue;
    if (parse_filter_list_command(
            &it, token::keeppackagenames, &pg_config->keeppackagenames))
      continue;
    if (parse_dontpreverify(&it, &pg_config->dontpreverify)) continue;
    if (parse_verbose(&it, &pg_config->verbose)) continue;
    if (parse_repackageclasses(&it)) continue;

    if (parse_filter_list_command(&it, token::dontwarn, &pg_config->dontwarn))
      continue;
    if (parse_filter_list_command(
            &it, token::keepattributes, &pg_config->keepattributes))
      continue;
    if (parse_class_specification_command(&it, token::assumenosideeffects))
      continue;

    // Skip unknown token.
    if ((*it)->is_command()) {
      cerr << "Unimplemented command (skipping): " << (*it)->show()
           << " at line " << (*it)->line << endl;
    } else {
      cerr << "Unexpected token " << (*it)->show() << " at line " << (*it)->line
           << endl;
    }
    (*parse_errors)++;
    it++;
    skip_to_next_command(&it);
  }
}

void parse(istream& config, ProguardConfiguration* pg_config) {

  std::vector<unique_ptr<Token>> tokens = lex(config);
  bool ok = true;
  // Check for bad tokens.
  for (auto& tok : tokens) {
    if (tok->type == token::unknownToken) {
      std::string spelling =
          static_cast<UnknownToken*>(tok.get())->token_string;
      ok = false;
    }
    // std::cout << tok->show() << " at line " << tok->line << std::endl;
  }
  unsigned int parse_errors = 0;
  if (ok) {
    parse(tokens.begin(), tokens.end(), pg_config, &parse_errors);
  }

  if (parse_errors == 0) {
    pg_config->ok = ok;
  } else {
    pg_config->ok = false;
    cerr << "Found " << parse_errors << " parse errors\n";
  }
}

void parse_file(const std::string filename, ProguardConfiguration* pg_config) {

  ifstream config(filename);
  if (!config.is_open()) {
    cerr << "Failed to open ProGuard configuration file " << filename << endl;
    pg_config->ok = false;
  }

  cout << "Reading ProGuard configuration from " << filename << endl;
  parse(config, pg_config);
  cout << "Done [" << filename << "]\n";
}

std::string show_bool(bool b) {
  if (b) {
    return "true";
  } else {
    return "false";
  }
}

} // namespace proguard_parser
} // namespace redex
