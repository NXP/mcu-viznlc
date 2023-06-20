# OASIS LITE runtime API

## Version Information

| **Name**       | **Type** | **Description**          |
| -------------- | -------- | ------------------------ |
| VERSION_MAJOR  | int      | The major version number |
| VERSION_MINOR  | int      | The minor version number |
| VERSION_HOTFIX | int      | Hot fix version number   |

## Function enable flags
These flags are used in library initializing procedure, caller can combine these flags to enable different functions.

| **Name**                               | **Bit index** | **Description**          |
| -------------------------------------- | ------------- | ------------------------ |
| OASIS_ENABLE_LIVENESS                  | 1<<0          | Enable/Disable liveness check in face recognition and registration |
| OASIS_ENABLE_MULTI_VIEW                | 1<<1          | Enable/Disable multi-view feature in face registration |
| OASIS_ENABLE_FACE_REC_BRIGHTNESS_CHECK | 1<<2          | Enable/Disable face brightness check during face quality check |
| OASIS_ENABLE_FACE_REC_BLUR_CHECK       | 1<<3          | Enable/Disable blur check on face recognition |
| OASIS_ENABLE_MASK_FACE_REC             | 1<<4          | Enable/Disable mask face recognition, only valid in special library with mask face support |
| OASIS_ENABLE_FACE_FEA_SMART_LEARNING   | 1<<5          | Enable/Disable face feature smart learning during face recognition |
| OASIS_ENABLE_DUP_FACE_REGISTRATION     | 1<<6          | Enable/Disable duplicate face registration |

## Result enumerations

These enumerations are used as return of OASIS LITE APIs and indicate the meaning of result.

| **Name**                                  | **Bit index** |  **Description**                   |
| ----------------------------------------- | ----------    | ---------------------------------- |
| OASISLT_OK                                | 0             | Result is successful.             |
| OASIS_INIT_INVALID_PARAMETERS             | 1             | Invalid input parameters.         |
| OASIS_INIT_INVALID_MODEL_CLASS            | 2             | Invalid input model class.        |
| OASIS_INIT_ALREADY_INIT                   | 3             | OASIS library already initialized.|
| OASIS_INIT_INVALID_FAR                    | 4             | Invalid false accept rate parameter.  |
| OASIS_INIT_INVALID_CB                     | 5             | Invalid callback parameter. |
| OASIS_INIT_MEMORYPOOL_SMALL               | 6             | Input memory pool is too small.  |
| OASIS_INIT_INVALID_MEMORYPOOL             | 7             | Input memory pool is NULL.  |
| OASIS_INIT_INVALID_IMAGE_MIN_DIM          | 8             | Invalid input frame height/width.  |
| OASIS_INIT_INVALID_MASK_BUF               | 9             | Invalid buffer size found for mask face recognition. |
| OASIS_INIT_INVALID_IMG_TYPE_FOR_MASK_FACE | 10            | Image type is not supported for mask face recognition. |
| OASIS_INIT_MASK_REC_NOTSUPORTED           | 11            | Mask face recognition is not supported in this library. |
| OASIS_INIT_INVALID_IMAGE_TYPE             | 12            | Invalid image type parameter. |
| OASIS_INIT_INSTANCE_NUM_OVERFLOW          | 13            | Instance number overflow/underflow.|
| OASIS_INIT_AUTHENTICATION_FAIL            | 14            | Authentication failed. |
| OASIS_INIT_MEM_ALLOCATE_FAIL              | 15            | Memory allocation error during initializing.|
| OASISLT_SNAPSHOT_INVALID_INPUT_PARAMETERS | 16            | Invalid input parameter in OASISlT_snapshot2Feature. |
| OASISLT_SNAPSHOT_LIB_UNINIT               | 17            | OASIS LITE library uninitialized before calling of OASISlT_snapshot2Feature. |
| OASISLT_SNAPSHOT_INVALID_FRAME_NUM        | 18            | In function OASISlT_snapshot2Feature, frame number unmatched. |
| OASISLT_SNAPSHOT_IMG_TYPE_NOT_SUPPORT     | 19            | In function OASISlT_snapshot2Feature, wrong image type. |
| OASISLT_SNAPSHOT_RESIZE_FAILED            | 20            | In function OASISlT_snapshot2Feature, snapshot resize error. |
| OASIS_INIT_INVALID_FASTMEMORYBUFFER       | 21            | OASIS LITE library fast buffer initialized error. |

## Run mode flag

This flag is used in OASISLT_run_extend function, it indicates what jobs is going to execute on the current input frame data.

| **Name**                     | **Type** | **Value** | **Description**                           |
| ---------------------------- | -------- | --------- | ----------------------------------------- |
| OASIS_DET_ONLY               | uint8_t  | 0         | Do face detection only.                   |
| OASIS_DET_WITH_QUALITY_CHECK | uint8_t  | 1         | Do face detection with face quality check.|
| OASIS_DET_REC                | uint8_t  | 2         | Do face detection, quality check and face recognition.   |
| OASIS_DET_REC_REG            | uint8_t  | 3         | Do face detection, quality check, face recognition and registration.   |
| OASIS_DET_REC_DEREG          | uint8_t  | 4         | Do face detection, quality check, face recognition and deregistration. |
| OASIS_DET_REC_REG_REMOTE     | uint8_t  | 5         | Do face detection, quality check, face recognition and registration with pictures. |

## Recognition result

This value indicates the result of face recognition. User can get this value from parameters of face recognition complete event.

| **Name**                     | **Type** | **Value** | **Description**                  |
| ---------------------------- | -------- | --------- | -------------------------------- |
| OASIS_REC_RESULT_KNOW_FACE   | uint8_t  | 0         | A matched face is found.         |
| OASIS_REC_RESULT_UNKNOW_FACE | uint8_t  | 1         | No face matched.                 |
| OASIS_REC_RESULT_INVALID     | uint8_t  | 0xFF      | Invalid face recognition result. |

## Registration result

This value indicates the result of face registration. User can get this value from parameters of face registration complete event.

| **Name**                      | **Type** | **Value** | **Description**              |
| ----------------------------- | -------- | --------- | ---------------------------- |
| OASIS_REG_RESULT_OK           | uint8_t  | 0         | Face registration success.   |
| OASIS_REG_RESULT_DUP          | uint8_t  | 1         | Face registration failed, this face has been registered in DB. |
| OASIS_REG_RESULT_CANCELED     | uint8_t  | 2         | Face registration has been cancelled by caller.  |
| OASIS_REG_RESULT_DB_OP_FAILED | uint8_t  | 3         | Face registration failed because of database operation failed. |
| OASIS_REG_RESULT_LOWER_COHESION | uint8_t  | 4       | Face registration failed because of lower cohesion. |
| OASIS_REG_RESULT_INVALID      | uint8_t  | 0xFF      | Failed with unknown reason.    |

## Deregistration result

This value indicates the result of face deregistration. User can get this value from parameters of face deregistration complete event.

| **Name**                        | **Type** | **Value** | **Description**              |
| ------------------------------- | -------- | --------- | ---------------------------- |
| OASIS_DEREG_RESULT_OK           | uint8_t  | 0         | Face deregistration success.   |
| OASIS_DEREG_RESULT_CANCELED     | uint8_t  | 1         | Face deregistration has been cancelled by caller. |
| OASIS_DEREG_RESULT_DB_OP_FAILED | uint8_t  | 2         | Face deregistration failed because of database operation failed. |
| OASIS_DEREG_RESULT_INVALID      | uint8_t  | 0xFF      | Failed with unknown reason. |

## Face quality result

Face quality result value list, these values will be returned in face quality check complete event notification.

| **Name**                                          | **Type** | **Value** | **Description**                                                           |
| ------------------------------------------------- | -------- | --------- | ------------------------------------------------------------------------- |
| OASIS_QUALITY_RESULT_FACE_OK                      | uint8_t  | 0         | Face quality is fine for recognition/registration. For internal use only. |
| OASIS_QUALITY_RESULT_FACE_TOO_SMALL               | uint8_t  | 1         | Face area is too small for recognition/registration.                      |
| OASIS_QUALITY_RESULT_FACE_ORIENTATION_UNMATCH     | uint8_t  | 2         | Face posture is side face.                                                |
| OASIS_QUALITY_RESULT_FACE_BLUR                    | uint8_t  | 3         | Face image is blur.                                                       |
| OASIS_QUALITY_RESULT_IR_FAKE                      | uint8_t  | 4         | Face detected in IR image is fake. Used in RGB+IR dual-camera case.       |
| OASIS_QUALITY_RESULT_RGB_FAKE                     | uint8_t  | 5         | Face detected in RGB image is fake. Used in RGB+IR dual-camera case.      |
| OASIS_QUALITY_RESULT_2D_FAKE                      | uint8_t  | 6         | Face detected in Depth image is fake. Used in 3D camera case.             |
| OASIS_QUALITY_RESULT_3D_FAKE                      | uint8_t  | 7         | Face detected in IR image is fake. Used in 3D camera case.                |
| OASIS_QUALITY_RESULT_DEPTH_INVALID                | uint8_t  | 8         | Face depth is invalid. Used in 3D camera case.                            |
| OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_DARK         | uint8_t  | 9         | Face used for recognition is too dark.                                    |
| OASIS_QUALITY_RESULT_FAIL_BRIGHTNESS_OVEREXPOSURE | uint8_t  | 10        | Face used for recognition is over exposure.                               |
| OASIS_QUALITY_RESULT_FACE_WITH_MASK               | uint8_t  | 11        | Face with mask.                                                           |
| OASIS_QUALITY_RESULT_SPECKLE_DISORDER             | uint8_t  | 12        | Not used.                                                                 |
| OASIS_QUALITY_RESULT_IR_DISORDER                  | uint8_t  | 13        | Not used.                                                                 |
| OASIS_QUALITY_RESULT_SPECKLE_NEAR_BORDER          | uint8_t  | 14        | Face in speckle image is too near border.                                 |
| OASIS_QUALITY_RESULT_INVALID                      | uint8_t  | 0xFF      | Unknown result.                                                           |

## Glasses check result

These values indicate whether a detected face is wearing a glasses or not.

| **Name**                                        | **Type** | **Value** | **Description**               |
| ----------------------------------------------- | -------- | --------- | ----------------------------- |
| OASIS_GLASSES_CHECK_RESULT_FACE_WITHOUT_GLASSES | uint8_t  | 0         | Detected face without glasses |
| OASIS_GLASSES_CHECK_RESULT_FACE_WITH_GLASSES    | uint8_t  | 1         | Detected face with glasses    |
| OASIS_GLASSES_CHECK_RESULT_INVALID              | uint8_t  | 2         | Unknown                       |

## Mask check result

These values indicate whether detected face is wearing a mask or not. Used in a library that supports mask recognition. 

| **Name**                                  | **Type** | **Value** | **Description**            |
| ----------------------------------------- | -------- | --------- | -------------------------- |
| OASIS_MASK_CHECK_RESULT_FACE_WITHOUT_MASK | uint8_t  | 0         | Detected face without mask |
| OASIS_MASK_CHECK_RESULT_FACE_WITH_MASK    | uint8_t  | 1         | Detected face with mask    |
| OASIS_MASK_CHECK_RESULT_INVALID           | uint8_t  | 2         | Unknown                    |

## FAR values

These values indicate what false accept rate can be acceptable in face recognition procedure. A lower FAR means a higher TRR(true rejection rate).

| **Name**            | **Type** | **Value** | **Description**   |
| ------------------- | -------- | --------- | ----------------- |
| OASIS_FAR_1_10000   | uint8_t  | 0         | one 10000th FAR   |
| OASIS_FAR_1_100000  | uint8_t  | 1         | one 100000th FAR  |
| OASIS_FAR_1_1000000 | uint8_t  | 2         | one 1000000th FAR |

## Model class

This flag indicates which model is going to be used in face recognition. Heavy model means a higher accuracy and more time cost for one face recognition.

| **Name**                  | **Type** | **Value** | **Description**                 |
| ------------------------- | -------- | --------- | ------------------------------- |
| OASISLT_MODEL_CLASS_LIGHT | uint8_t  | 0         | A light face recognition model  |
| OASISLT_MODEL_CLASS_HEAVY | uint8_t  | 1         | A heavy face recognition model. |

## Image formats

This value indicates color format, bits for each color and their storage sequence in memory. For all images, we assume pixel format is HWC.

| **Name**                 | **Type** | **Value** | **Description**                                                                                                 |
| ------------------------ | -------- | --------- | --------------------------------------------------------------------------------------------------------------- |
| OASIS_IMG_FORMAT_RGB888  | uint8_t  | 0         | 3 bytes for each pixel, 8 bits for each color(Red, Green, and Blue) and storage sequence is red, green and blue |
| OASIS_IMG_FORMAT_BGR888  | uint8_t  | 1         | Same as above but the storage sequence is blue, green and red.                                                  |
| OASIS_IMG_FORMAT_GREY888 | uint8_t  | 2         | 3 bytes for each pixel and they are same. For internal use only                                                 |
| OASIS_IMG_FORMAT_GREY8   | uint8_t  | 3         | 1 bytes for each pixel and this byte is grey value. For internal use only                                       |

## Image types

These values indicate input image stream configuration to OASIS library. It indicates what type of frames input and handled by OASISLT_run_extend API.

| **Name**                        | **Type** | **Value** | **Description**                                                                  |
| ------------------------------- | -------- | --------- | -------------------------------------------------------------------------------- |
| OASIS_IMG_TYPE_RGB_SINGLE       | uint8_t  | 0         | Single RGB image stream                                                          |
| OASIS_IMG_TYPE_IR_SINGLE        | uint8_t  | 1         | Single IR image stream                                                           |
| OASIS_IMG_TYPE_IR_RGB_DUAL      | uint8_t  | 2         | Both IR and RGB image stream. Do face recognition on IR image.                   |
| OASIS_IMG_TYPE_RGB_IR_DUAL      | uint8_t  | 3         | Both IR and RGB image stream. Do face recognition on RGB image.                  |
| OASIS_IMG_TYPE_IR_RGB_3D_TRIPLE | uint8_t  | 4         | IR,RGB,3D images are all input. Do face recognition on IR image.                 |
| OASIS_IMG_TYPE_RGB_IR_3D_TRIPLE | uint8_t  | 5         | IR,RGB,3D images are all input. Do face recognition on RGB image.                |

## Speckle template types

These values indicate what type of speckle template is used for 3D depth decorder.

| **Name**                                  | **Type** | **Value** | **Description**                 |
| ----------------------------------------- | -------- | --------- | ------------------------------- |
| OASIS_TEMPLATE_TYPE_PREPROCESSED          | uint8_t  | 0         | Normal preprocessed template    |
| OASIS_TEMPLATE_TYPE_PREPROCESSED_SUBPIXEL | uint8_t  | 1         | Subpixel preprocessed template  |

## Frame index

These indexes are used to differentiate IR,RGB and 3D frames.

| **Name**                   | **Type** | **Value** | **Description**    |
| -------------------------- | -------- | --------- | ------------------ |
| OASISLT_INT_FRAME_IDX_RGB  | uint8_t  | 0         | RGB frame index    |
| OASISLT_INT_FRAME_IDX_IR   | uint8_t  | 1         | IR frame index     |
| OASISLT_INT_FRAME_IDX_3D   | uint8_t  | 2         | 3D frame index     |
| OASISLT_INT_FRAME_IDX_LAST | uint8_t  | 3         | Frame index number |

## Landmark index

These indexes are used to indicate coordinate of 5 landmark on the detected face.

| **Name**                        | **Type** | **Value** | **Description**           |
| ------------------------------- | -------- | --------- | ------------------------- |
| OASISLT_LM_LEFT_EYE_X           | uint8_t  | 0         | Left eye X                |
| OASISLT_LM_RIGHT_EYE_X          | uint8_t  | 1         | Right eye x               |
| OASISLT_LM_NOSE_X               | uint8_t  | 2         | Nose x                    |
| OASISLT_LM_MOUTH_LEFT_CORNER_X  | uint8_t  | 3         | Mouth left corner x       |
| OASISLT_LM_MOUTH_RIGHT_CORNER_X | uint8_t  | 4         | Mouth right corner x      |
| OASISLT_LM_LEFT_EYE_Y           | uint8_t  | 5         | Left eye Y                |
| OASISLT_LM_RIGHT_EYE_Y          | uint8_t  | 6         | Right eye Y               |
| OASISLT_LM_NOSE_Y               | uint8_t  | 7         | Nose Y                    |
| OASISLT_LM_MOUTH_LEFT_CORNER_Y  | uint8_t  | 8         | Mouth left corner Y       |
| OASISLT_LM_MOUTH_RIGHT_CORNER_Y | uint8_t  | 9         | Mouth right corner Y      |
| OASISLT_LM_IDX_NUM              | uint8_t  | 10        | All landmark index number |

About 106 landmark points, please see the figure as below.

<!--
<fig>
<title> 106 Landmark Points</title>
<image href="img/landmark106.png">
</image></fig>
-->
![106 landmark points](img/landmark106.png)

## Face Box structure

Structure name: FBox, it indicates face ROI area and landmark information.

| **Name** | **Type**                        | **Description**                                       |
| -------- | ------------------------------- | ----------------------------------------------------- |
| rect     | Int[4]                        | The left, top, right, bottom of the face bounding box |
| fld      | Float[OASISLT_LM_IDX_NUM]    | The x, y coordinate of the 5 face landmark points     |
| fld106   | Float[ OASISLT_LM106_IDX_NUM] | The x, y coordinate of the 106 face landmark points   |

## Image frame structure

Structure name: ImageFrame_t.

| **Name** | **Type**             | **Description**                                                                                              |
| -------- | -------------------- | ------------------------------------------------------------------------------------------------------------ |
| height   | short                | Image height in pixel                                                                                        |
| width    | short                | Image width in pixel                                                                                         |
| fmt      | OASISLTImageFormat_t | indicate the format of this frame                                                                            |
| data     | unsigned char\*      | Pointer of image data. Image data format is HWC format. Specially, for 3D frame, format is 16bits per pixel. |

## Event IDs

In procedure of OASISLT_run_extend, when a job(face detection/recognition, emotion recognition, face quality check and face registration) is started/completed or/and in progress, caller will be notified by an event callback function with a given event ID, this event list is as below.

| **Name**                         | **Type** | **Value** | **Description**                                |
| -------------------------------- | -------- | --------- | ---------------------------------------------- |
| OASISLT_EVT_DET_START            | uint8_t  | 0         | Face detection start.                          |
| OASISLT_EVT_DET_COMPLETE         | uint8_t  | 1         | Face detection complete.                       |
| OASISLT_EVT_QUALITY_CHK_START    | uint8_t  | 2         | Face picture quality check start.              |
| OASISLT_EVT_QUALITY_CHK_COMPLETE | uint8_t  | 3         | Face picture quality check complete.           |
| OASISLT_EVT_REC_START            | uint8_t  | 4         | Face recognition start.                        |
| OASISLT_EVT_REC_COMPLETE         | uint8_t  | 5         | Face recognition complete.                     |
| OASISLT_EVT_REG_START            | uint8_t  | 6         | Face registration start.                       |
| OASISLT_EVT_REG_IN_PROGRESS      | uint8_t  | 7         | Face registration procedure go forward a step. |
| OASISLT_EVT_REG_COMPLETE         | uint8_t  | 8         | Face registration complete.                    |
| OASISLT_EVT_DEREG_START          | uint8_t  | 9         | Face deregistration start.                     |
| OASISLT_EVT_DEREG_COMPLETE       | uint8_t  | 10        | Face deregistration complete.                  |

## Event callback parameter structure

Structure name: OASISLTCbPara_t

| **Name**        | **Type**                     | **Description**                                                                                                                                                                                                                |
| --------------- | ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| faceBoxIR       | FBox\*                       | Face box detected on IR image stream.NULL indicates that no face is detected.                                                                                                                                                  |
| faceBoxRGB      | FBox\*                       | Face box detected on RGB image stream.NULL indicates that no face is detected.                                                                                                                                                 |
| faceID          | uint16_t                     | For Face recognition complete event callback, this ID indicates recognized face ID of pfaceBox.For Registration complete event callback, this ID is the new generated face ID for pfaceBox.For other event, faceID is invalid. |
| regResult       | OASISLTRegisterRes_t         | Result of registration. It is only valid when registration complete event happens.                                                                                                                                             |
| faceOrientation | OASISLTFaceOrientation_t     | Valid for face registration in progress event                                                                                                                                                                                                                       |
| deregResult     | OASISLTDeregisterRes_t       | Face deregistration result.                                                                                                                                                                                                    |
| qualityResult   | OASISLTFaceQualityRes_t      | Face quality result, only valid when OASISLT_EVT_QUALITY_CHK_COMPLETE event happens.                                                                                                                                           |
| recResult       | OASISLTRecognizeRes_t        | Result of face recognition. Only valid when face recognition complete event happens.                                                                                                                                           |
| glassesResult   | OASISLTFaceGlassesCheckRes_t | Result of glasses detetion.                                                                                                                                                                                                    |
| reserved        | int[32]                    | This int array is reserved for internal debug purpose only.                                                                                                                                                                    |

## Instance handler

**Prototype:**

```c
Typedef void* OASISLTHandler_t;
```

**Description :**

This type is used to define a OASIS LITE library instance handler which should be used for all multi-thread APIs for the 1st parameter. Detail information on how to use it please refer to multi instance support chapter.

## Event callback function

**Prototype**:

```c
typedef void (*OASISLTEvtCb)(ImageFrame_t* frames[OASISLT_INT_FRAME_IDX_LAST],
                             OASISLTEvt_t evt,
                             OASISLTCbPara_t* para,
                             void* userData);
```

**Description :**

In the process of OASISLT_run_extend, this callback function be triggered when each event in the event list happens. Frames is same to "frames" input parameter in OASISLT_run_extend.

| **Parameter Name** | **Input/Output** | **Description**                                                                                     |
| ------------------ | ---------------- | --------------------------------------------------------------------------------------------------- |
| frames             | Input            | Current image frames on which all jobs is handling.                                                 |
| evt                | Input            | Event ID which triggers this callback.                                                              |
| para               | Input            | Other parameters which input to this callback, see _Event callback parameter structure_ for detail. |
| userData           | Input            | User data pointer in calling of OASISLT_run_extend                                                  |

## Get registered faces function

**Prototype**:

```c
typedef int (*GetRegisteredFaces)(uint16_t* faceId, void** pFaces, uint32_t* faceNum, void* userData);
```

**Description :**

This function should be implemented by the caller. By this interface, OASIS LITE runtime library can get all face information records (ID and feature pointers).

Return 0 if success, other value indicate a failure.

| **Parameter Name** | **Input/Output** | **Description**                                                                                                                                                                                                                                                                                                                               |
| ------------------ | ---------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| faceId             | Output           | It points to a uint16_t array buffer, this buffer is going to be used to save all face IDs returned. Each ID identified an unique face information record. The size of this buffer is allocated by OASIS lite library, it is   \*face_num X sizeof(uint16_t)                                                                                  |
| pfaces             | Output           | It points to an array buffer which is used to save all pointers of face information record. Each record has a pointer and these pointers should be sync with ID array in face_id. OASIS lite library will use these pointers to read face features.Size of this buffer is: \*face_num X sizeof(void\*)                                        |
| faceNum            | Input/Output     | For input, it indicate how many face ID and record pointers OASIS lite library requires. Specially, when it is set to 0 In this case, the actual face records number in database should be set to \*Face_num and face ID and pFace pointer array can be NULL.For output, it indicates how many face information records are returned in fact. |
| userData           | Input            | User data pointer in calling of OASISLT_run_extend.                                                                                                                                                                                                                                                                                           |

## Face add function

**Prototype**:

```c
typedef int (*FaceOperationAdd)( uint16_t* faceId, void* faceData, SnapshotItem_t* snapshotData, int snapshotNum, void* userData);
```

**Description :**

This function should be implemented by the caller. By this interface, OASIS LITE runtime library can add face information record to face record database. Face IDs are maintained by external database, each ID should be unique. When a record of face information is added successfully, a unique ID should be generated and returned to OASIS runtime library.

Snapshot usually is used to generate new face information record when current feature records can not be used because of library upgrading. Called may ignore this information if upgrading is not cared.

Return 0 if success, other value indicate a failure.

| **Parameter Name** | **Input/Output** | **Description**                                                                                                                                             |
| ------------------ | ---------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| faceId             | Output           | Face ID generated for this face information record if adding successfully.                                                                                  |
| faceData           | Input            | Pointer to a face information record. It’s size can be get byOASISLT_getFaceItemSize()                                                                      |
| snapshotData       | Input            | Snapshot packet associated with this face record. By this snapshot data, caller can generate face information record again by API OASISLT_snapshot2feature. |
| snapshotNum        | Input            | Snapshot packet count                                                                                                                                       |
| userData           | Input            | User data pointer in calling of OASISLT_run_extend                                                                                                          |

## Face update function

**Prototype**:

```c
typedef int (*FaceOperationUpdate)(uint16_t faceId, void* faceData, SnapshotItem_t* snapshotData, int snapshotNum, void* userData);
```

**Description :**

This function should be implemented by the caller. By this interface, OASIS LITE runtime library can update face information record and corresponding snapshot data by face ID.

Return 0 if success, other value indicate a failure.

| **Parameter Name** | **Input/Output** | **Description**                                                                        |
| ------------------ | ---------------- | -------------------------------------------------------------------------------------- |
| faceId             | Input            | face information record ID to be updated.                                              |
| faceData           | Input            | Pointer to a face information record. It’s size can be get byOASISLT_getFaceItemSize() |
| snapshotData       | Input            | Part of Snapshot data associated with this face                                        |
| snapshotNum        | Input            | Snapshot data count                                                                    |
| userData           | Input            | User data pointer in calling of OASISLT_run_extend                                     |

## Face delete function

**Prototype:**

```c
typedef int (*FaceOperationDelete)(uint16_t faceId, void* userData);
```

**Description:**

This function should be implemented by the caller. By this interface, OASIS LITE runtime library can delete face information record and corresponding snapshot data by face ID.

Return 0 if success, other value indicate a failure.

## Brightness adjustment function

**Prototype**:

```c
typedef int (*FaceBrightnessAdjust)(uint8_t frameIdx,uint8_t direction,void* userData);
```

**Description :**

This function should be implemented by the caller. By this interface, OASIS LITE runtime library can adjust brightness on given image stream.

Return 0 if success, other value indicate a failure.

| **Parameter Name** | **Input/Output** | **Description**                                                                            |
| ------------------ | ---------------- | ------------------------------------------------------------------------------------------ |
| frameIdx           | Input            | On which camera, brightness adjustment is needed. It can be one of RGB/IR/3D frame.         |
| direction          | Input            | 0: adjust the brightness to a little darker.1: adjust the brightness to a little brighter. |
| userData           | Input            | User data pointer in calling of OASISLT_run_extend                                     |

## Initializing parameter structure

**Description :**

Initializing parameter is the a parameter OASISLT_init need. It’s structure as below:

| **Parameter Name** | **Type** | **Description**                                                                                                                                                                                                                               |
| ------------------ | -------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| height             | Input    | Maximum height of input images OASIS library can handle. It should not less than height of any frame input.                                                                                                                                    |
| width              | Input    | Maximum width of input images OASIS library can handle. It should not less than width of any frame input                                                                                                                                       |
| imgType            | Input    | Image stream type input to OASIS runtime library.This type indicates how many frames are input to OASIS_run_extend API once a time and it also indicate on which frame face recognition should be done. |
| isSpeckle          | Input    | Check if the 3D frame is Depth16 or Speckle16, when using 3D imgType. |
| templateType       | Input    | Speckle template type when using Speckle16 3D imgtype. |
| speckleTemplate    | Input    | Pointer to the speckle template, when using Speckle16 3D imgtype. |
| minFace            | Input    | Minimum size of the face which can be detected. This size should less or equal to the value of "min_face" parameter for each call of OASISLT_run. Additionally, it should be >= 40.                                                           |
| memPool            | Input    | Memory pool can be used by OASIS library.If this parameter is NULL, OASISLT_init would return memory pool size required.                                                                                                                      |
| size               | Input    | Memory pool size.                                                                                                                                                                                                                             |
| fastMemBuf         | Input    | Fast memory buffer supply for oasis library.                                                                                                                                                                                                  |
| fastMemSize        | Input    | Fast memory buffer size.                                                                                                                                                                                                                      |
| cbs                | Input    | Callback function set, which includes:<br>1.Event callback function<br>2.Get registered faces function<br>3.Add face function<br>4.Update face function<br>5.Delete face function<br>6.Adjust brightness function<br>7.Reserved (internal debugging use only) |
| enableFlags | Input | What functions are enabled in OASIS Library?Refer to chapter "function enable flags" |
| falseAcceptRate | Input | False accept rate caller required. |
| modClass | Input | Light or heavy model should be used for face recognition.Heavy model means a higher accuracy and more time cost. It is not valid for mask face recognition. |
| runtimePara | Input | This define customer's all kind of algorithm threshold. Please don't configure it before you know the meaning of each parameter clearly. |

## Callback functions structure

**Description :**

This structure is used to set all callback functions needed during OASIS LITE runtime. It’s structure as below:

| **Parameter Name** | **Type**             | **Description**                                                                                                                                                                                                                                                                                                 |
| ------------------ | -------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| EvtCb              | OASISLTEvtCb         | This callback function is called when any event start/complete inside library occurred. It can be NULL if caller does not care about any event.                                                                                                                                                                  |
| GetFaces           | GetRegisteredFaces   | By this function, library can get how many face are registered or get IDs and features of registered faces. It can not be NULL.                                                                                                                                                                                  |
| AddFace            | FaceOperationAdd     | By this function, library can save a face item record or snapshot to somewhere( it depend on caller, can be ram/disk). Caller can also save them to a temporary place instead of face DB and call OASISLT_registration_by_feature to add them to DB later at anytime. If is NULL, no face item should be saved. |
| UpdateFace         | FaceOperationUpdate  | By this function,library can update a face item. if is NULL, no update will be done. If face registration is done by remote (on PC or app on mobile phone), we strongly recommend to provide this callback.                                                                                                     |
| DeleteFace         | FaceOperationDelete  | By this function,library can delete a face item. if is NULL, no update will be done.                                                                                                                                                                                                                            |
| AdjustBrightness   | FaceBrightnessAdjust | If this function exist, library will call it to improve the image quality when darkness/over exposure are found on RGB and IR image.                                                                                                                                                                            |
| lock               | EnterCriticalArea    | This function is to used for multi-thread environment when multiple OASISLT_run_extend instances are running and some critical area need be protected such like memory allocation/free.                                                                                                                         |
| unlock             | ExitCriticalArea     | Same as above                                                                                                                                                                                                                                                                                                   |
| reserved           | Void\*               | Reserved for internal use only.                                                                                                                                                                                                                                                                                 |
