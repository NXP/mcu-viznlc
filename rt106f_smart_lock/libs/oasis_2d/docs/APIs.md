# APIs

## OASISLT_init

**Prototype**:

```c
OASISLTResult_t OASISLT_init(OASISLTInitPara_t* para);
```
**Description :**

Initialize OASIS LITE runtime library. If mem_pool is NULL, memory size needed is set to para->size.

Return 0 if success, other value indicates a failure. See "Result enumerations".

| **Parameter Name** | **Input/Output** | **Description**|
| ------------------ | ---------------- | -------------- |
|para                | Input/Output     |Refer to "initializing parameter structure" for detail information. |

## OASISLT_run_extend

**Prototype**:

```c
int OASISLT_run_extend(ImageFrame_t* frames[OASISLT_INT_FRAME_IDX_LAST],  
                       uint8_t flag, 
                       int minFace,
                       void* userData);
```

**Description :**

Do Jobs(face detection/recognition, and face registration, up to flag parameter) on given image frames.

This function can support single/dual/triple frames input.

Return 0 if success, other value indicates a failure.

| **Parameter Name** | **Input/Output** | **Description**|
| ------------------ | ---------------- | -------------- |
|frames              | Input            |Refer to "initializing parameter structure" for detail information.         |
|flag                | Input            |What jobs is going to take? Refer to “Run mode flag” for detail information.         |
|minFace             | Input            |The minimum face size can be detected on the current image frames, it should not less than minFace in library initializing parameters.|
|userData            |Input             |User data transfer to callback functions.|

## OASISLT_uninit

**Prototype**:
```c
OASISLTResult_t OASISLT_uninit(); 
```
**Description :**

Uninitialize OASIS LITE runtime library.

Return 0 if success, other value indicates a failure. See "Result enumerations".


## OASISLT_run_identification

**Prototype**:
```c
int OASISLT_run_identification(ImageFrame_t* input, 
                               ImageFrame_t* target,
                               float* sim);
```

**Description :**

Used to compare and calculate the similarity of faces in 2 frames.

Return 0 if success, other value indicates a failure. See "Identify Result enumerations".

| **Parameter Name** | **Input/Output** | **Description**|
| ------------------ | ---------------- | -------------- |
|input               | Input            |input image frame.  |
|target              | Input            |target image frame. |
|sim                 | Output           |output similarity.   |


## OASISLT_registration_by_feature

**Prototype**:

```c
OASISLTRegisterRes_t OASISLT_registration_by_feature(
                    void* faceData,
                    void* snapshot,
                    int snapshotLength,
                    uint16_t* id, 
                    void* userData);
```

**Description :**

Register a face by face feature. This function usually used by remote registration when a feature is got from remote side and need be added in local face feature database.

Return 0 if success, other value indicates a failure. See “Register Result enumerations”.


| **Parameter Name** | **Input/Output** | **Description**|
| ------------------ | ---------------- | -------------- |
|faceData               | Input            |Face feature data, it’s length should be OASISLT_getFaceItemSize().  |
|snapshot              | Input            |snapshot associated with this face data. It is optional, can be set to NULL if not used. |
|snapshotLength              | Input            |length of snapshot. |
|id                 | Output           |face ID generated after this face feature added successfully.   |
|userData                 | Input           |User data transfer to callback functions.   |


## OASISLT_MT_run_extend

**Prototype**:

```c
int OASISLT_MT_run_extend(OASISLTHandler_t handler,
                          ImageFrame_t* frames[OASISLT_INT_FRAME_IDX_LAST],
                          uint8_t flag,
                          int minFace,
                          void* userData);
```

**Description :**

Muti-thread version of OASISLT_run_extend, parameter handler indicates which instance is running.


## OASISLT_MT_run_identification

```c
int OASISLT_MT_run_identification(OASISLTHandler_t handler,
                                  ImageFrame_t* input,
                                  ImageFrame_t* target,
                                  float* sim);
```

**Description :**

Muti-thread version of OASISLT_run_identification, parameter handler indicates which instance is running.

## OASISLT_MT_registration_by_feature

```c
OASISLTRegisterRes_t OASISLT_MT_registration_by_feature(
                     OASISLTHandler_t handler,
                     void* faceData,
                     void* snapshot,
                     int snapshotLength, 
                     uint16_t* id, 
                     void* userData);
```

**Description :**

Muti-thread version of OASISLT_registration_by_feature, parameter handler indicates which instance is running.

## OASISLT_CreateInstance

```c
OASISLTResult_t OASISLT_CreateInstance(OASISLTHandler_t* pHandler);
```


**Description :**

Create an OASIS LITE instance.

pHandler: [output] instance pointer used to save created instance.

Return 0 if success, other value indicates a failure. See "Result enumerations".



## OASISLT_DeleteInstance

```c
OASISLTResult_t OASISLT_DeleteInstance(OASISLTHandler_t handler);
```

**Description :**

Delete an OASIS LITE instance.

handler: [input] instance handler to delete.

Return 0 if success, other value indicates a failure. See "Result enumerations".


## OASISLT_getVersion

```c
void OASISLT_getVersion(char* verStrBuf, int length);
```

**Description :**

Get version information of OASIS lite runtime library.

| **Parameter Name** | **Input/Output** | **Description**|
| ------------------ | ---------------- | -------------- |
|verStrBuf           | Input/Output     |buffer used to save version information. |
|length              | Input            |buffer length, at least 64 bytes are needed. |


## OASISLT_getFaceItemSize

```c
uint32_t OASISLT_getFaceItemSize(void);
```

**Description :**

When adding/updating face information record or get face information records, caller need know face information item size by this function.

Return face information record size in bytes.
