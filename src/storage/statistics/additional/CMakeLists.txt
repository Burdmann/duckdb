add_library_unity(
  duckdb_storage_statistics_additional
  OBJECT
  )
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_storage_statistics_additional>
    PARENT_SCOPE)
