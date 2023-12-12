#ifndef DECLARE_EXAMPLES
#define DECLARE_EXAMPLES

// Legacy examples
void basicReflectionExamples();
void globalReflectionExample();
void annotatedSupersExample();
void objectMapperExamples();
void dynamicJsonIoExample();
void jsonCustomizationExample();
void functionReflectionExample();
void reflectionLimitsExample();

// Member examples
inline namespace member_define_iterate_modify { void memberDefineIterateModify(); }
inline namespace member_access_by_constexpr_index { void memberAccessByConstexprIndex(); }
inline namespace member_access_by_runtime_index { void memberAccessByRuntimeIndex(); }
inline namespace member_access_by_name { void memberAccessByName(); }
inline namespace member_access_protection_modifier { void memberAccessProtectionModifier(); }
inline namespace member_metadata_by_identifier { void memberMetadataByIdentifier(); }
inline namespace member_pack_fold { void memberPackFold(); }
inline namespace member_filter_constexpr_if { void memberFilterConstexprIf(); }
inline namespace member_filter_predicate { void memberFilterPredicate(); }
inline namespace member_reflect_outside_of_class { void memberReflectOutsideOfClass(); }

// Unowned private examples
inline namespace unowned_private_reflect { void unownedPrivateReflect(); }
inline namespace unowned_private_noted { void unownedPrivateReflectNoted(); }

// Aggregate examples
inline namespace auto_reflect_aggregate { void autoReflectAggregate(); }

// Note examples
inline namespace note_class_members { void noteClassMembers(); }
inline namespace note_class { void noteClass(); }
inline namespace note_anonymous { void noteAnonymous(); }
inline namespace note_filtered { void noteFiltered(); }

// Super-class examples
inline namespace super_define_iterate { void superDefineIterate(); }
inline namespace super_access_by_constexpr_index { void superAccessByConstexprIndex(); }
inline namespace super_access_by_runtime_index { void superAccessByRuntimeIndex(); }
inline namespace super_traverse_tree { void superTraverseTree(); }

// Function examples
inline namespace function_define_iterate_call { void functionDefineIterateCall(); }
inline namespace function_build_and_call { void functionBuildAndCall(); }

// Overload examples
inline namespace overload_define_iterate_call { void overloadDefineIterateCall(); }
inline namespace overload_access_by_constexpr_index { void overloadAccessByConstexprIndex(); }
inline namespace overload_access_by_runtime_index { void overloadAccessByRuntimeIndex(); }
inline namespace overload_pack_expand { void overloadPackExpand(); }
inline namespace overload_non_noted_argument_sets { void overloadNonNotedArgumentSets(); }
inline namespace overload_cvref_qualifications { void overloadCvrefQualifications(); }

// Misc examples
inline namespace misc_direct_tuplification { void miscDirectTuplification(); }
inline namespace misc_type_to_string { void miscTypeToString(); }
inline namespace misc_runtime_to_constexpr_index { void miscRuntimeToConstexprIndex(); }
inline namespace misc_tuplify { void miscTuplify(); }
inline namespace misc_whitebox_testing { void miscWhiteboxTesting(); }
inline namespace misc_make_tuple { void miscMakeTuple(); }

// Adaptive structure examples
inline namespace adaptive_clone_structure { void adaptiveCloneStructure(); }
inline namespace adaptive_typedef_structure { void adaptiveTypedefStructure(); }
inline namespace adaptive_implement_whiteboxer { void adaptiveImplementWhiteboxer(); }
inline namespace adaptive_implement_builder { void adaptiveImplementBuilder(); }
inline namespace adaptive_implement_change_listener { void adaptiveImplementChangeListener(); }
inline namespace adaptive_implement_mapper { void adaptiveImplementMapper(); }

// Builder examples
inline namespace builder_simple { void builderSimple(); }
inline namespace builder_validated { void builderValidated(); }
inline namespace builder_aggregated { void builderAggregated(); }
inline namespace builder_member_availability { void builderMemberAvailability(); }

// Mapper examples
inline namespace mapper_trivial_mapping { void mapperTrivialMapping(); }
inline namespace mapper_custom_field_mapping { void mapperCustomFieldMapping(); }
inline namespace mapper_using_standard_mappings { void mapperUsingStandardMappings(); }

// Json examples
inline namespace json_basic { void jsonBasic(); }
inline namespace json_customizations { void jsonCustomizations(); }
inline namespace json_unknown_fields { void jsonUnknownFields(); }

#endif