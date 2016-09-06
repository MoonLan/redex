/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_NEWLINE = 258,
     T_SEMICOLON = 259,
     T_COMMA = 260,
     T_NOT = 261,
     T_AT = 262,
     T_QUOTE = 263,
     T_COMMENT = 264,
     T_KEEP = 265,
     T_KEEPNAMES = 266,
     T_KEEPCLASSMEMBERS = 267,
     T_KEEPCLASSMEMBERNAMES = 268,
     T_KEEPCLASSESWITHMEMBERS = 269,
     T_KEEPCLASSESWITHMEMBERNAMES = 270,
     T_ALLOWOBFUSCATION = 271,
     T_ALLOWOPTIMIZATION = 272,
     T_ALLOWSHRINKING = 273,
     T_ADAPTCLASSSTRINGS = 274,
     T_ADAPTRESOURCEFILECONTENTS = 275,
     T_ADAPTRESOURCEFILENAMES = 276,
     T_ALLOWACCESSMODIFICATION = 277,
     T_APPLYMAPPING = 278,
     T_ASSUMENOSIDEEFFECTS = 279,
     T_CLASSOBFUSCATIONDICTIONARY = 280,
     T_DONTNOTE = 281,
     T_DONTOBFUSCATE = 282,
     T_DONTOPTIMIZE = 283,
     T_DONTPREVERIFY = 284,
     T_DONTSHRINK = 285,
     T_DONTWARN = 286,
     T_DONTUSEMIXEDCASECLASSNAMES = 287,
     T_DONTSKIPNONPUBLICLIBRARYCLASSES = 288,
     T_FLATTENPACKAGEHIERARCHY = 289,
     T_INJARS = 290,
     T_INCLUDE = 291,
     T_KEEPATTRIBUTES = 292,
     T_KEEPPACKAGENAMES = 293,
     T_KEEPPARAMETERNAMES = 294,
     T_LIBRARYJARS = 295,
     T_MERGEINTERFACESAGGRESSIVELY = 296,
     T_OBFUSCATIONDICTIONARY = 297,
     T_OPTIMIZATIONPASSES = 298,
     T_OPTIMIZATIONS = 299,
     T_OUTJARS = 300,
     T_OVERLOADAGGRESSIVELY = 301,
     T_PACKAGEOBFUSCATIONDICTIONARY = 302,
     T_PRINTCONFIGURATION = 303,
     T_PRINTMAPPING = 304,
     T_PRINTSEEDS = 305,
     T_PRINTUSAGE = 306,
     T_RENAMESOURCEFILEATTRIBUTE = 307,
     T_REPACKAGECLASSES = 308,
     T_USEUNIQUECLASSMEMBERNAMES = 309,
     T_VERBOSE = 310,
     T_WHYAREYOUKEEPING = 311,
     T_CLASS = 312,
     T_ENUM = 313,
     T_INTERFACE = 314,
     T_AT_INTERFACE = 315,
     T_INIT = 316,
     T_IMPLEMENTS = 317,
     T_EXTENDS = 318,
     T_PUBLIC = 319,
     T_PRIVATE = 320,
     T_PROTECTED = 321,
     T_STATIC = 322,
     T_FINAL = 323,
     T_TRANSIENT = 324,
     T_NATIVE = 325,
     T_METHODS = 326,
     T_FIELDS = 327,
     T_ANY_MEMBER = 328,
     T_PATTERN = 329,
     T_MEMBERS_BEGIN = 330,
     T_MEMBERS_END = 331,
     T_ARGS_BEGIN = 332,
     T_ARGS_END = 333
   };
#endif
/* Tokens.  */
#define T_NEWLINE 258
#define T_SEMICOLON 259
#define T_COMMA 260
#define T_NOT 261
#define T_AT 262
#define T_QUOTE 263
#define T_COMMENT 264
#define T_KEEP 265
#define T_KEEPNAMES 266
#define T_KEEPCLASSMEMBERS 267
#define T_KEEPCLASSMEMBERNAMES 268
#define T_KEEPCLASSESWITHMEMBERS 269
#define T_KEEPCLASSESWITHMEMBERNAMES 270
#define T_ALLOWOBFUSCATION 271
#define T_ALLOWOPTIMIZATION 272
#define T_ALLOWSHRINKING 273
#define T_ADAPTCLASSSTRINGS 274
#define T_ADAPTRESOURCEFILECONTENTS 275
#define T_ADAPTRESOURCEFILENAMES 276
#define T_ALLOWACCESSMODIFICATION 277
#define T_APPLYMAPPING 278
#define T_ASSUMENOSIDEEFFECTS 279
#define T_CLASSOBFUSCATIONDICTIONARY 280
#define T_DONTNOTE 281
#define T_DONTOBFUSCATE 282
#define T_DONTOPTIMIZE 283
#define T_DONTPREVERIFY 284
#define T_DONTSHRINK 285
#define T_DONTWARN 286
#define T_DONTUSEMIXEDCASECLASSNAMES 287
#define T_DONTSKIPNONPUBLICLIBRARYCLASSES 288
#define T_FLATTENPACKAGEHIERARCHY 289
#define T_INJARS 290
#define T_INCLUDE 291
#define T_KEEPATTRIBUTES 292
#define T_KEEPPACKAGENAMES 293
#define T_KEEPPARAMETERNAMES 294
#define T_LIBRARYJARS 295
#define T_MERGEINTERFACESAGGRESSIVELY 296
#define T_OBFUSCATIONDICTIONARY 297
#define T_OPTIMIZATIONPASSES 298
#define T_OPTIMIZATIONS 299
#define T_OUTJARS 300
#define T_OVERLOADAGGRESSIVELY 301
#define T_PACKAGEOBFUSCATIONDICTIONARY 302
#define T_PRINTCONFIGURATION 303
#define T_PRINTMAPPING 304
#define T_PRINTSEEDS 305
#define T_PRINTUSAGE 306
#define T_RENAMESOURCEFILEATTRIBUTE 307
#define T_REPACKAGECLASSES 308
#define T_USEUNIQUECLASSMEMBERNAMES 309
#define T_VERBOSE 310
#define T_WHYAREYOUKEEPING 311
#define T_CLASS 312
#define T_ENUM 313
#define T_INTERFACE 314
#define T_AT_INTERFACE 315
#define T_INIT 316
#define T_IMPLEMENTS 317
#define T_EXTENDS 318
#define T_PUBLIC 319
#define T_PRIVATE 320
#define T_PROTECTED 321
#define T_STATIC 322
#define T_FINAL 323
#define T_TRANSIENT 324
#define T_NATIVE 325
#define T_METHODS 326
#define T_FIELDS 327
#define T_ANY_MEMBER 328
#define T_PATTERN 329
#define T_MEMBERS_BEGIN 330
#define T_MEMBERS_END 331
#define T_ARGS_BEGIN 332
#define T_ARGS_END 333




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

