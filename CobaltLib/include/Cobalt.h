
/*******************************************************************************
 * Cobalt.h
 * - public API
 ******************************************************************************/
#ifndef COBALT_H
#define COBALT_H

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#endif
#ifdef _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif

#if Cobalt_BACKEND_OPENCL12
#include "CL/cl.h"
typedef cl_float2 CobaltComplexFloat;
typedef cl_double2 CobaltComplexDouble;
#else
#include <hip_runtime.h>
#if (defined( __GNUC__ ) || defined( __IBMC__ ))
    #define Cobalt_ALIGNED(_x) __attribute__ ((aligned(_x)))
#else
    #define Cobalt_ALIGNED(_x)
#endif

typedef union {
   float  Cobalt_ALIGNED(8) s[2];
   struct{ float  x, y; };
   struct{ float  s0, s1; };
} CobaltComplexFloat;

typedef union {
   double  Cobalt_ALIGNED(8) s[2];
   struct{ double  x, y; };
   struct{ double  s0, s1; };
} CobaltComplexDouble;

#endif


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * CobaltStatus
 ******************************************************************************/
typedef enum CobaltStatus_ {

  // success
  cobaltStatusSuccess = 0,

  /* VALIDATION ERRORS */
  cobaltStatusValidationErrorMin,
  
  /* cobaltValidateProblem() */
  cobaltStatusProblemIsNull,

  // tensor errors
  cobaltStatusTensorNumDimensionsInvalid,
  cobaltStatusTensorDimensionOrderInvalid,
  cobaltStatusTensorDimensionStrideInvalid,
  cobaltStatusTensorDimensionSizeInvalid,
  
  // operation errors
  cobaltStatusOperandNumDimensionsMismatch,
  cobaltStatusOperationOperandNumIndicesMismatch,
  cobaltStatusOperationNumIndicesMismatch,
  cobaltStatusOperationIndexAssignmentInvalidA,
  cobaltStatusOperationIndexAssignmentInvalidB,
  cobaltStatusOperationIndexAssignmentDuplicateA,
  cobaltStatusOperationIndexAssignmentDuplicateB,
  cobaltStatusOperationNumIndicesInvalid,
  cobaltStatusOperationNumFreeIndicesInvalid,
  cobaltStatusOperationNumSummationIndicesInvalid,
  cobaltStatusOperationIndexUnassigned,
  cobaltStatusOperationFreeIndexAssignmentsInvalid,
  cobaltStatusOperationBatchIndexAssignmentsInvalid,
  cobaltStatusOperationSummationIndexAssignmentsInvalid,

  // device profile errors
  cobaltStatusDeviceProfileDeviceNameInvalid,

  /* cobaltGetSolution() */
  cobaltStatusOperationTypeNotFound,
  cobaltStatusDeviceProfileNumDevicesInvalid,
  cobaltStatusDeviceProfileNotFound,
  cobaltStatusProblemNotSupported, // purposefully not supported
  cobaltStatusProblemNotFound, // should be supported but wasn't found
  cobaltStatusSolutionDoesNotSupportOffsets, // tried to construct a Solution which doesn't support TensorData.offsets using a Problem which does require them
  cobaltStatusSolutionDoesNotSupportLeadingStrides, // tried to construct a Solution which doesn't support leading strides using a Problem which does have leading strides

  /* control errors */
  cobaltStatusControlInvalid,
  cobaltStatusDependencyInvalid,

  /* misc */
  cobaltStatusParametersInvalid,

  cobaltStatusValidationErrorMax,
  cobaltStatusPerformanceWarningMin,

  /* Performance Warnings */

  /* cobaltEnqueueSolution() */
  cobaltStatusPerformanceWarningProblemSizeTooSmall, // ?

  cobaltStatusPerformanceWarningMax,

} CobaltStatus;


/*******************************************************************************
 * cobaltStatusCheck
 * prints whether status is error, warning or success and status string
 ******************************************************************************/
#define cobaltStatusCheck(status) \
  if (cobaltStatusIsError(status) || cobaltStatusIsError(status)) { \
    unsigned int _cobaltStatusStringSize; \
    cobaltStatusToString( status, nullptr, &_cobaltStatusStringSize); \
    char *_cobaltStatusString = new char[_cobaltStatusStringSize]; \
    cobaltStatusToString(status, _cobaltStatusString, &_cobaltStatusStringSize); \
    printf("CobaltStatus::%s::%s on line %u of %s\n", \
      cobaltStatusIsError(status) ? "Error" : "Warning", \
      _cobaltStatusString, \
      __LINE__, \
      __FILE__); \
    delete[] _cobaltStatusString; \
  }


/*******************************************************************************
 * cobaltStatusIsError - status is an error
 ******************************************************************************/
bool cobaltStatusIsError( CobaltStatus status );


/*******************************************************************************
* cobaltStatusIsWarning - status is a performance warning
******************************************************************************/
bool cobaltStatusIsWarning( CobaltStatus status );


/*******************************************************************************
 * cobaltSetup & cobaltTeardown
 * logFileName is c-string of where to write log file
 ******************************************************************************/
CobaltStatus cobaltSetup( const char *logFilePath );
CobaltStatus cobaltTeardown();


/*******************************************************************************
 * CobaltDataType
 ******************************************************************************/
typedef enum CobaltDataType_ {
  cobaltDataTypeHalf,                   // 0
  cobaltDataTypeSingle,                 // 1
  cobaltDataTypeDouble,                 // 2
  cobaltDataTypeComplexHalf,            // 3
  cobaltDataTypeComplexSingle,          // 4
  cobaltDataTypeComplexDouble,          // 5
  cobaltDataTypeComplexConjugateHalf,   // 6
  cobaltDataTypeComplexConjugateSingle, // 7
  cobaltDataTypeComplexConjugateDouble, // 8
  cobaltNumDataTypes,                   // 9
  cobaltDataTypeNone,                   // 10
} CobaltDataType;


/*******************************************************************************
 * CobaltDimension
 ******************************************************************************/
typedef struct CobaltDimension_ {
  unsigned int stride;
  unsigned int size;
} CobaltDimension;


/*******************************************************************************
 * CobaltTensor
 ******************************************************************************/
typedef struct CobaltTensor_ {
  CobaltDataType dataType;
  enum { maxDimensions = 16 } maxDimensions_;
  unsigned int numDimensions;
  CobaltDimension dimensions[maxDimensions];
} CobaltTensor;

/*******************************************************************************
* cobaltCreateEmptyTensor
* - returns CobaltTensor initialized to zero
******************************************************************************/
CobaltTensor cobaltCreateEmptyTensor();

/*******************************************************************************
 * Tensor Data - OpenCL 1.2
 ******************************************************************************/
#if Cobalt_BACKEND_OPENCL12
#include "CL/cl.h"

typedef struct CobaltTensorData_ {
  void *data;
  unsigned int offset;
} CobaltTensorData;
typedef struct CobaltTensorDataConst_ {
  const void *data;
  unsigned int offset;
} CobaltTensorDataConst;


/*******************************************************************************
 * Tensor Data - HIP
 ******************************************************************************/
#elif Cobalt_BACKEND_HIP
typedef struct CobaltTensorData_ {
  void *data;
  unsigned int offset;
} CobaltTensorData;
typedef struct CobaltTensorDataConst_ {
  const void *data;
  unsigned int offset;
} CobaltTensorDataConst;

#endif

typedef struct CobaltScalarData_ {
  const void *data;
} CobaltScalarData;


/*******************************************************************************
 * Device
 * the device on which the problem is to be computed
 ******************************************************************************/
typedef struct CobaltDevice_ {
  enum { maxNameLength = 256 } maxNameLength_;
  char name[maxNameLength];
} CobaltDevice;


/*******************************************************************************
 * CobaltDeviceProfile
 * describes the device(s) on which the problem is to be computed
 ******************************************************************************/
typedef struct CobaltDeviceProfile_ {
  enum { maxDevices = 1 } maxDevices_;
  unsigned int numDevices;
  CobaltDevice devices[maxDevices];
} CobaltDeviceProfile;

/*******************************************************************************
* cobaltCreateEmptyDeviceProfile
* returns CobaltDeviceProfile initialized to zero
******************************************************************************/
CobaltDeviceProfile cobaltCreateEmptyDeviceProfile();

/*******************************************************************************
 * CobaltOperationType
 ******************************************************************************/
typedef enum CobaltOperationType_ {
  cobaltOperationTypeContraction,
  cobaltOperationTypeConvolution
  //cobaltOperationTypeCorrelation
} CobaltOperationType;


/*******************************************************************************
 * CobaltControl
 * controls the execution of the solution (queue, dependencies, dependents)
 ******************************************************************************/
typedef struct CobaltControl_ {
  void *validate;
  unsigned int benchmark;
  enum { maxQueues = 16 } maxQueues_;
  unsigned int numQueues;
  unsigned int numQueuesUsed; // by library
  unsigned int numInputEvents;
  unsigned int numOutputEvents;
#if Cobalt_BACKEND_OPENCL12
  cl_command_queue queues[maxQueues];
  cl_event *inputEvents;
  cl_event *outputEvents;
#elif Cobalt_BACKEND_HIP
  hipStream_t queues[maxQueues];
  hipEvent_t *inputEvents;
  hipEvent_t *outputEvents;
#endif
} CobaltControl;



/*******************************************************************************
* cobaltCreateEmptyControl
* returns CobaltControl initialized to zero
******************************************************************************/
CobaltControl cobaltCreateEmptyControl();


/*******************************************************************************
 * CobaltProblem
 * problem describes the computation to be performed
 ******************************************************************************/
typedef struct _CobaltProblem * CobaltProblem;


/*******************************************************************************
* CobaltSolution
* solution describes how problem will be computed
*   kernels to be enqueued
*   kernel parameters
*   kernel thread range
******************************************************************************/
typedef struct _CobaltSolution * CobaltSolution;


/*******************************************************************************
 * cobaltCreateProblem
 * creates CobaltProblem object
 * buffer pointers are not specified here
 ******************************************************************************/
CobaltProblem cobaltCreateProblem(
    CobaltTensor tensorC,
    CobaltTensor tensorA,
    CobaltTensor tensorB,
    unsigned int *indexAssignmentsA,
    unsigned int *indexAssignmentsB,
    CobaltOperationType operationType,
    CobaltDataType alphaType,
    CobaltDataType betaType,
    bool useOffsets,
    CobaltDeviceProfile deviceProfile,
    CobaltStatus *status );
CobaltStatus cobaltDestroyProblem( CobaltProblem problem );


/*******************************************************************************
 * cobaltValidateProblem
 * checks that problem is self consistent
 *   number of tensor dimensions
 *   free indices
 *   batch indices
 *   summation indices
 *   indexAssignments
 ******************************************************************************/
CobaltStatus cobaltValidateProblem( CobaltProblem problem );


/*******************************************************************************
 * cobaltGetSolutionForProblem
 * returns optimal solution for input problem according to prior benchmarking
 ******************************************************************************/
CobaltSolution cobaltGetSolutionForProblem(
    const CobaltProblem problem,
    CobaltStatus *status );
CobaltStatus cobaltDestroySolution( CobaltSolution solution );


/*******************************************************************************
 * cobaltEnqueueSolution
 *   enqueues solution
 *   buffer pointers are specified here
 ******************************************************************************/
CobaltStatus cobaltEnqueueSolution(
    CobaltSolution solution,
    CobaltTensorData tensorDataC,
    CobaltTensorDataConst tensorDataA,
    CobaltTensorDataConst tensorDataB,
    CobaltScalarData alpha,
    CobaltScalarData beta,
    CobaltControl *control );


/*******************************************************************************
 * cobalt*ToString
 * get c-string representation of objects
 * if size is non-null, it is set to size if string user needs to allocate
 * if cstr is non-null, string is written to cstr buffer
 ******************************************************************************/
CobaltStatus cobaltStatusToString(
    CobaltStatus status, char *cstr, unsigned int *size );
CobaltStatus cobaltProblemToString(
    CobaltProblem problem, char *cstr, unsigned int *size );
CobaltStatus cobaltSolutionToString(
    CobaltSolution solution, char *cstr, unsigned int *size );



#ifdef __cplusplus
} // extern "C"
#endif

#endif // COBALT_H
