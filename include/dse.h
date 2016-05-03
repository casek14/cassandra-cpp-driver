/*
  Copyright (c) 2014-2016 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef __DSE_H_INCLUDED__
#define __DSE_H_INCLUDED__

#include <cassandra.h>

#if !defined(DSE_STATIC)
#  if (defined(WIN32) || defined(_WIN32))
#    if defined(DSE_BUILDING)
#      define DSE_EXPORT __declspec(dllexport)
#    else
#      define DSE_EXPORT __declspec(dllexport)
#    endif
#  elif (defined(__SUNPRO_C)  || defined(__SUNPRO_CC)) && !defined(DSE_STATIC)
#    define DSE_EXPORT __global
#  elif (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
#    define DSE_EXPORT __attribute__ ((visibility("default")))
#  endif
#else
#define DSE_EXPORT
#endif

#if defined(_MSC_VER)
#  define DSE_DEPRECATED(func) __declspec(deprecated) func
#elif defined(__GNUC__) || defined(__INTEL_COMPILER)
#  define DSE_DEPRECATED(func) func __attribute__((deprecated))
#else
#  define DSE_DEPRECATED(func) func
#endif

/**
 * @file include/dse.h
 *
 * C/C++ Driver Extensions for DataStax Enterprise
 */

#define DSE_VERSION_MAJOR 1
#define DSE_VERSION_MINOR 0
#define DSE_VERSION_PATCH 0
#define DSE_VERSION_SUFFIX "alpha"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Graph options for executing graph queries.
 *
 * @struct DseGraphOptions
 */
typedef struct DseGraphOptions_ DseGraphOptions;

/**
 * Graph statement for executing graph queries. This represents a graph query
 * string, graph options and graph values used to execute a graph query.
 *
 * @struct DseGraphStatement
 */
typedef struct DseGraphStatement_ DseGraphStatement;

/**
 * Graph object builder for constructing a collection of member pairs.
 *
 * @struct DseGraphObject
 */
typedef struct DseGraphObject_ DseGraphObject;

/**
 * Graph array builder for constructing an array of elements.
 *
 * @struct DseGraphArray
 */
typedef struct DseGraphArray_ DseGraphArray;

/**
 * Graph result set
 *
 * @struct DseGraphResultSet
 */
typedef struct DseGraphResultSet_ DseGraphResultSet;

/**
 * Graph result types
 */
typedef enum DseGraphResultType_ {
  DSE_GRAPH_RESULT_TYPE_NULL,
  DSE_GRAPH_RESULT_TYPE_BOOL,
  DSE_GRAPH_RESULT_TYPE_NUMBER,
  DSE_GRAPH_RESULT_TYPE_STRING,
  DSE_GRAPH_RESULT_TYPE_OBJECT,
  DSE_GRAPH_RESULT_TYPE_ARRAY
} DseGraphResultType;

/**
 * Graph result
 *
 * @struct DseGraphResult
 */
typedef struct DseGraphResult_ DseGraphResult;

/**
 * Graph edge
 *
 * @struct DseGraphEdgeResult
 */
typedef struct DseGraphEdgeResult_ {
  const DseGraphResult* id;
  const DseGraphResult* label;
  const DseGraphResult* type;
  const DseGraphResult* properties;
  const DseGraphResult* in_vertex;
  const DseGraphResult* in_vertex_label;
  const DseGraphResult* out_vertex;
  const DseGraphResult* out_vertex_label;
} DseGraphEdgeResult;

/**
 * Graph vertex
 *
 * @struct DseGraphVertexResult
 */
typedef struct DseGraphVertexResult_ {
  const DseGraphResult* id;
  const DseGraphResult* label;
  const DseGraphResult* type;
  const DseGraphResult* properties;
} DseGraphVertexResult;

/**
 * Graph path
 *
 * @struct DseGraphPathResult
 */
typedef struct DseGraphPathResult_ {
  const DseGraphResult* labels;
  const DseGraphResult* objects;
} DseGraphPathResult;

/***********************************************************************************
 *
 * Cluster
 *
 ***********************************************************************************/

/**
 *
 */
DSE_EXPORT CassError
cass_cluster_set_dse_gssapi_authenticator(CassCluster* cluster,
                                          const char* service,
                                          const char* principal);

/**
 *
 */
DSE_EXPORT CassError
cass_cluster_set_dse_gssapi_authenticator_n(CassCluster* cluster,
                                            const char* service,
                                            size_t service_length,
                                            const char* principal,
                                            size_t principal_length);

/**
 *
 */
DSE_EXPORT CassError
cass_cluster_set_dse_plaintext_authenticator(CassCluster* cluster,
                                             const char* username,
                                             const char* password);

/**
 *
 */
DSE_EXPORT CassError
cass_cluster_set_dse_plaintext_authenticator_n(CassCluster* cluster,
                                               const char* username,
                                               size_t username_length,
                                               const char* password,
                                               size_t password_length);

/***********************************************************************************
 *
 * Session
 *
 ***********************************************************************************/

/**
 * Execute a graph statement.
 *
 * @public @memberof CassSession
 *
 * @param[in] session
 * @param[in] statement
 * @return A future that must be freed.
 *
 * @see cass_future_get_dse_graph_resultset()
 */
DSE_EXPORT CassFuture*
cass_session_execute_dse_graph(CassSession* session,
                               const DseGraphStatement* statement);

/***********************************************************************************
 *
 * Future
 *
 ***********************************************************************************/

/**
 * Gets the graph result set of a successful future. If the future is not ready this method will
 * wait for the future to be set.
 *
 * @public @memberof CassFuture
 *
 * @param[in] future
 * @return DseGraphResultSet instance if successful, otherwise NULL for error. The return instance
 * must be freed using dse_graph_resultset_free().
 *
 * @see cass_session_execute_dse_graph()
 */
DSE_EXPORT DseGraphResultSet*
cass_future_get_dse_graph_resultset(CassFuture* future);

/***********************************************************************************
 *
 * Graph Options
 *
 ***********************************************************************************/

/**
 * Creates a new instance of graph options
 *
 * @public @memberof DseGraphOptions
 *
 * @return Returns a instance of graph options that must be freed.
 *
 * @see dse_graph_options_free()
 */
DSE_EXPORT DseGraphOptions*
dse_graph_options_new();

/**
 * Frees a graph options instance.
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 */
DSE_EXPORT void
dse_graph_options_free(DseGraphOptions* options);

/**
 * Set the graph language to be used in graph queries.
 *
 * Default: gremlin-groovy
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] language
 * @return CASS_OK if successful, otherwise an error occurred.
 *
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_language(DseGraphOptions* options,
                                     const char* language);

/**
 * Same as dse_graph_options_set_graph_language(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] language
 * @param[in] language_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_language_n(DseGraphOptions* options,
                                       const char* language, size_t language_length);

/**
 * Set the graph traversal source name to be used in graph queries.
 *
 * Default: default
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] source
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_source(DseGraphOptions* options,
                                   const char* source);

/**
 * Same as dse_graph_options_set_graph_source(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] source
 * @param[in] source_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_source_n(DseGraphOptions* options,
                                     const char* source, size_t source_length);

/**
 * Set the graph name to be used in graph queries. This is optional and the
 * name is left unset if this function is not called.
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] name
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_name(DseGraphOptions* options,
                                 const char* name);

/**
 * Same as dse_graph_options_set_graph_name(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphOptions
 *
 * @param[in] options
 * @param[in] name
 * @param[in] name_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_options_set_graph_name_n(DseGraphOptions* options,
                                   const char* name, size_t name_length);

/***********************************************************************************
 *
 * Graph Statement
 *
 ***********************************************************************************/

/**
 * Creates a new instance of graph statement.
 *
 * @public @memberof DseGraphStatement
 *
 * @param[in] query
 * @param[in] options Optional. Use NULL for a system query with the
 * default graph language and source.
 * @return Returns a instance of graph statement that must be freed.
 */
DSE_EXPORT DseGraphStatement*
dse_graph_statement_new(const char* query,
                        const DseGraphOptions* options);

/**
 * Same as dse_graph_statement_new_n(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphStatement
 *
 * @param[in] query
 * @param[in] query_length
 * @param[in] options Optional. Use NULL for a system query with the
 * default graph language and source.
 * @return Returns a instance of graph statement that must be freed.
 */
DSE_EXPORT DseGraphStatement*
dse_graph_statement_new_n(const char* query,
                          size_t query_length,
                          const DseGraphOptions* options);

/**
 * Frees a graph statement instance.
 *
 * @public @memberof DseGraphStatement
 *
 * @param[in] statement
 */
DSE_EXPORT void
dse_graph_statement_free(DseGraphStatement* statement);

/**
 * Bind the values to a graph query.
 *
 * @public @memberof DseGraphStatement
 *
 * @param[in] statement
 * @param[in] values
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_statement_bind_values(DseGraphStatement* statement,
                                const DseGraphObject* values);

/***********************************************************************************
 *
 * Graph Object
 *
 ***********************************************************************************/

/**
 * Creates a new instance of graph object.
 *
 * @public @memberof DseGraphObject
 */
DSE_EXPORT DseGraphObject*
dse_graph_object_new();

/**
 * Frees a graph object instance.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 */
DSE_EXPORT void
dse_graph_object_free(DseGraphObject* object);

/**
 * Reset a graph object. This function must be called after previously finishing
 * an object (dse_graph_object_finish()). This can be used to resuse an
 * instance of DseGraphObject to create multiple objects.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 */
DSE_EXPORT void
dse_graph_object_reset(DseGraphObject* object);

/**
 * Finish a graph object. This function must be called before adding an object to
 * another object, array or binding to a statement.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 */
DSE_EXPORT void
dse_graph_object_finish(DseGraphObject* object);

/**
 * Add null to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_null(DseGraphObject* object,
                          const char* name);

/**
 * Same as dse_graph_object_add_null(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_null_n(DseGraphObject* object,
                            const char* name,
                            size_t name_length);

/**
 * Add boolean to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_bool(DseGraphObject* object,
                          const char* name,
                          cass_bool_t value);

/**
 * Same as dse_graph_array_add_bool(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_bool_n(DseGraphObject* object,
                            const char* name,
                            size_t name_length,
                            cass_bool_t value);

/**
 * Add integer (32-bit) to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_int32(DseGraphObject* object,
                           const char* name,
                           cass_int32_t value);

/**
 * Same as dse_graph_object_add_int32(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_int32_n(DseGraphObject* object,
                             const char* name,
                             size_t name_length,
                             cass_int32_t value);

/**
 * Add integer (64-bit) to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_int64(DseGraphObject* object,
                           const char* name,
                           cass_int64_t value);

/**
 * Same as dse_graph_object_add_int64(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_int64_n(DseGraphObject* object,
                             const char* name,
                             size_t name_length,
                             cass_int64_t value);

/**
 * Add double to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_double(DseGraphObject* object,
                            const char* name,
                            cass_double_t value);

/**
 * Same as dse_graph_object_add_double(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_double_n(DseGraphObject* object,
                              const char* name,
                              size_t name_length,
                              cass_double_t value);

/**
 * Add string to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_string(DseGraphObject* object,
                            const char* name,
                            const char* value);

/**
 * Same as dse_graph_object_add_string(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @param[in] value_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_string_n(DseGraphObject* object,
                              const char* name,
                              size_t name_length,
                              const char* value,
                              size_t value_length);

/**
 * Add object to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_object(DseGraphObject* object,
                            const char* name,
                            const DseGraphObject* value);

/**
 * Same as dse_graph_object_add_object(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_object_n(DseGraphObject* object,
                              const char* name,
                              size_t name_length,
                              const DseGraphObject* value);

/**
 * Add array to an object with the specified name.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_array(DseGraphObject* object,
                           const char* name,
                           const DseGraphArray* value);

/**
 * Same as dse_graph_object_add_array(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphObject
 *
 * @param[in] object
 * @param[in] name
 * @param[in] name_length
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_object_add_array_n(DseGraphObject* object,
                             const char* name,
                             size_t name_length,
                             const DseGraphArray* value);

/***********************************************************************************
 *
 * Graph Array
 *
 ***********************************************************************************/

/**
 * Creates a new instance of graph array.
 *
 * @public @memberof DseGraphArray
 */
DSE_EXPORT DseGraphArray*
dse_graph_array_new();

/**
 * Frees a graph array instance.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 */
DSE_EXPORT void
dse_graph_array_free(DseGraphArray* array);

/**
 * Reset a graph array. This function must be called after previously finishing
 * an array (dse_graph_array_finish()). This can be used to resuse an
 * instance of DseGraphArray to create multiple arrays.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 */
DSE_EXPORT void
dse_graph_array_reset(DseGraphObject* array);

/**
 * Finish a graph array. This function must be called before adding an array to
 * another object, array or binding to a statement.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 */
DSE_EXPORT void
dse_graph_array_finish(DseGraphObject* array);

/**
 * Add null to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_null(DseGraphArray* array);

/**
 * Add boolean to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_bool(DseGraphArray* array,
                          cass_bool_t value);

/**
 * Add integer (32-bit) to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_int32(DseGraphArray* array,
                           cass_int32_t value);

/**
 * Add integer (64-bit) to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_int64(DseGraphArray* array,
                           cass_int64_t value);

/**
 * Add double to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_double(DseGraphArray* array,
                            cass_double_t value);

/**
 * Add string to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_string(DseGraphArray* array,
                            const char* value);

/**
 * Same as dse_graph_array_add_string(), but with lengths for string
 * parameters.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @param[in] value_length
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_string_n(DseGraphArray* array,
                            const char* value,
                             size_t value_length);

/**
 * Add object to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] valeu
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_object(DseGraphArray* array,
                           const DseGraphObject* value);

/**
 * Add array to an array.
 *
 * @public @memberof DseGraphArray
 *
 * @param[in] array
 * @param[in] value
 * @return CASS_OK if successful, otherwise an error occurred.
 */
DSE_EXPORT CassError
dse_graph_array_add_array(DseGraphArray* array,
                          const DseGraphArray* value);

/***********************************************************************************
 *
 * Graph Result Set
 *
 ***********************************************************************************/

/**
 * Frees a graph result set instance.
 *
 * @public @memberof DseGraphResultSet
 *
 * @param[in] resultset
 */
DSE_EXPORT void
dse_graph_resultset_free(DseGraphResultSet* resultset);

/**
 * Returns the number of results in the result set.
 *
 * @public @memberof DseGraphResultSet
 *
 * @param[in] resultset
 * @return The number of results in the result set.
 */
DSE_EXPORT size_t
dse_graph_resultset_count(DseGraphResultSet* resultset);

/**
 * Returns the next result in the result set.
 *
 * @public @memberof DseGraphResultSet
 *
 * @param[in] resultset
 * @return The next result.
 */
DSE_EXPORT const DseGraphResult*
dse_graph_resultset_next(DseGraphResultSet* resultset);

/***********************************************************************************
 *
 * Graph Result
 *
 ***********************************************************************************/

/**
 * Returns the type of the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The type of the current result.
 */
DSE_EXPORT DseGraphResultType
dse_graph_result_type(const DseGraphResult* result);

/**
 * Returns true if the result is a boolean.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is boolean, otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_bool(const DseGraphResult* result);

/**
 * Returns true if the result is an integer (32-bit).
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a number that be held in an integer (32-bit),
 * otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_int32(const DseGraphResult* result);

/**
 * Returns true if the result is an integer (64-bit).
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a number that be held in an integer (64-bit),
 * otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_int64(const DseGraphResult* result);

/**
 * Returns true if the result is a double.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a number that be held in a double,
 * otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_double(const DseGraphResult* result);

/**
 * Returns true if the result is a string.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a string, otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_string(const DseGraphResult* result);

/**
 * Returns true if the result is an object.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a object, otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_object(const DseGraphResult* result);

/**
 * Returns true if the result is an array.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return True if the result is a array, otherwise false.
 */
DSE_EXPORT cass_bool_t
dse_graph_result_is_array(const DseGraphResult* result);

/**
 * Get the boolean value from the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The boolean value.
 *
 */
DSE_EXPORT cass_bool_t
dse_graph_result_get_bool(const DseGraphResult* result);

/**
 * Get the integer (32-bit) value from the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The integer (32-bit) value.
 *
 */
DSE_EXPORT cass_int32_t
dse_graph_result_get_int32(const DseGraphResult* result);

/**
 * Get the integer (64-bit) value from the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The integer (64-bit) value.
 *
 */
DSE_EXPORT cass_int64_t
dse_graph_result_get_int64(const DseGraphResult* result);

/**
 * Get the double value from the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The double value.
 *
 */
DSE_EXPORT cass_double_t
dse_graph_result_get_double(const DseGraphResult* result);

/**
 * Get the string value from the result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @param[out] length
 * @return The string value.
 *
 */
DSE_EXPORT const char*
dse_graph_result_get_string(const DseGraphResult* result,
                            size_t* length);

/**
 * Return an object as an graph edge.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return CASS_OK if successful, otherwise an error occurred.
 *
 */
DSE_EXPORT CassError
dse_graph_result_as_edge(const DseGraphResult* result,
                         DseGraphEdgeResult* edge);

/**
 * Return an object as an graph vertex.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return CASS_OK if successful, otherwise an error occurred.
 *
 */
DSE_EXPORT CassError
dse_graph_result_as_vertex(const DseGraphResult* result,
                           DseGraphVertexResult* vertex);

/**
 * Return an object as an graph path.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return CASS_OK if successful, otherwise an error occurred.
 *
 */
DSE_EXPORT CassError
dse_graph_result_as_path(const DseGraphResult* result,
                         DseGraphPathResult* path);

/**
 * Returns the number of members in an object result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The number of members in an object result.
 *
 */
DSE_EXPORT size_t
dse_graph_result_member_count(const DseGraphResult* result);

/**
 * Return the string key of an object at the specified index.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The string key of the member.
 *
 */
DSE_EXPORT const char*
dse_graph_result_member_key(const DseGraphResult* result,
                            size_t index,
                            size_t* length);

/**
 * Return the result value of an object at the specified index.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The result value of the member.
 *
 */
DSE_EXPORT const DseGraphResult*
dse_graph_result_member_value(const DseGraphResult* result,
                              size_t index);

/**
 * Returns the number of elements in an array result.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The number of elements in array result.
 *
 */
DSE_EXPORT size_t
dse_graph_result_element_count(const DseGraphResult* result);

/**
 * Returns the result value of an array at the specified index.
 *
 * @public @memberof DseGraphResult
 *
 * @param[in] result
 * @return The result value.
 *
 */
DSE_EXPORT const DseGraphResult*
dse_graph_result_element(const DseGraphResult* result,
                         size_t index);

/***********************************************************************************
 *
 * GSSAPI Authentication
 *
 ***********************************************************************************/

/**
 *
 */
typedef void (*DseGssapiAuthenticatorLockCallback)(void* data);

/**
 *
 */
typedef void (*DseGssapiAuthenticatorUnockCallback)(void* data);

/**
 *
 */
DSE_EXPORT CassError
dse_gssapi_authenticator_set_lock_callbacks(DseGssapiAuthenticatorLockCallback lock_callback,
                                            DseGssapiAuthenticatorUnockCallback unlock_callback,
                                            void* data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __DSE_H_INCLUDED__ */
