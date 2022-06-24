**Camera_Test**

---

**Setup:**

*Import The Project:*

- Clone the repo

- Go to Android Studio > New > Import Project

*Setup OpenCV:*

- Create a new project with C++ Native template

> [Releases - OpenCV](https://opencv.org/releases/)

- Go here and download your preferred OpenCV android sdk version

- Go to File > New > Import Module

- Find and select the downloaded SDK in the file explorer

> Example: (Absolute path)
> 
> <arbitrary_location>/opencv-4.2.0-android-sdk/OpenCV-android-sdk/sdk

- Give the module a name for example ":opencv-4.2.0"

> OpenCV SDK should now be added to the project as a separate folder next to the app folder

- Go to your app's build.gradle and opencv's build.gradle and set compileSdk, buildTools, minSdk, targetSdk versions in opencv build.gradle the same as your app's build.gradle

- In app build.gradle add a dependency to OpenCV under dependencies section using the name you gave to your opencv module i.e. ":opencv-4.2.0"

> Example:
> 
> dependencies {
> 
>     ...
> 
> > implementation project(path: ':opencv-4.2.0')
> 
>     ...
> 
> }

- In app build.gradle add the lines below to your externalNativeBuild under defaultConfig in order to access OpenCV from Native C++

> externalNativeBuild {  
>      cmake {  
>          cppFlags "-frtti -fexceptions"  
>          abiFilters 'x86', 'x86_64', 'armeabi-v7a', 'arm64-v8a'  
>          arguments "-DOpenCV_DIR=" + opencvsdk + "/sdk/native"  
>      }  
> }

- Go to gradle.properties and add a variable named opencvsdk which holds the absolute path to your downloaded OpenCV Android sdk folder (Temporary fix)

> Example: (Absolute path)
> 
> opencvsdk=<arbitrary_location>/opencv-4.2.0-android-sdk/OpenCV-android-sdk

---

**Additional Information:**

*Camera Access:*

- Add into your AndroidManifest.xml this line before the application scope

< uses-permission android:name="android.permission.CAMERA"/ >

> *Lock Camera to Landscape:*

- In AndroidManifest.xml add these within the brackets of the activity scope

> android:screenOrientation="landscape"  
> tools:ignore="LockedOrientationActivity"

> Example:
> 
> <activity  
>  android:name=".MainActivity"  
>  android:exported="true"  
>  android:screenOrientation="landscape"  
>  tools:ignore="LockedOrientationActivity">

*Camera View:*

- Switch to code view in activity_main.xml and add this layout & view:

> <FrameLayout xmlns:android="http://schemas.android.com/apk/res/android"  
>      xmlns:tools="http://schemas.android.com/tools"  
>      xmlns:opencv="http://schemas.android.com/apk/res-auto"  
>      android:layout_width="match_parent"  
>      android:layout_height="match_parent"  
>      android:id="@+id/frameLayout">  
> 
>  <org.opencv.android.JavaCameraView android:layout_width="wrap_content"  
>      android:layout_height="wrap_content"  
>      android:visibility="gone"  
>      android:id="@+id/opencv_camera_view"  
>      opencv:show_fps="true"  
>      opencv:camera_id="any"  
>      tools:layout_conversion_absoluteHeight="0dp"  
>      tools:layout_editor_absoluteY="0dp"  
>      tools:layout_editor_absoluteX="0dp"  
>      tools:layout_conversion_absoluteWidth="0dp" />  
> 
> </FrameLayout>

*OpenCV C++ Setup:*

- Add this to your CMake below project and above add_library

> include_directories(\${OpenCV_DIR}/jni/include)
> add_library( lib_opencv SHARED IMPORTED )
> set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION \${OpenCV_DIR}/libs/\${ANDROID_ABI}/libopencv_java4.so)

- Add lib_opencv into target_link_libraries

> Example:
> 
> target_link_libraries( # Specifies the target library.
>         barcode_camera_test
>         lib_opencv
>         # Links the target library to the log library
>         # included in the NDK.
>         \${log-lib})

- In native-lib.cpp, add:

> #include "opencv2/core.hpp"  
> #include "opencv2/imgproc.hpp"  
> 
> using namespace cv;

- and add the function (with barcode_1camera_1test part modified with your project name):

> extern "C" JNIEXPORT void JNICALL  
> Java_com_example_barcode_1camera_1test_MainActivity_grayscale(JNIEnv *env, jobject /* this */, jlong addrImg, jlong addrOut) {  
>     Mat& out=*(Mat*)addrOut;  
>  Mat& img=*(Mat*)addrImg;  
>  cvtColor(img, out, COLOR_RGBA2GRAY);  
> }

*MainActivity Setup:*

- Add this whole class as your main activity and include all imports, however at the very bottom, change the library name in System.loadLibrary to your project library name i.e. "barcode_camera_test":

> class MainActivity : CameraActivity(), CvCameraViewListener2 {
>     var frame = Mat()
>     var gray = Mat()
>     private var mOpenCvCameraView: CameraBridgeViewBase? = null
>     private val mIsJavaCamera = true
>     private val mItemSwitchCamera: MenuItem? = null
>     private val mLoaderCallback: BaseLoaderCallback = object : BaseLoaderCallback(this) {
>         override fun onManagerConnected(status: Int) {
>             when (status) {
>                 SUCCESS -> {
>                     Log.i(TAG, "OpenCV loaded successfully")
>                     mOpenCvCameraView!!.enableView()
>                 }
>                 else -> {
>                     super.onManagerConnected(status)
>                 }
>             }
>         }
>     }
> 
>     fun Tutorial1Activity() {
>         Log.i(TAG, "Instantiated new " + this.javaClass)
>     }
>     
>     public override fun onCreate(savedInstanceState: Bundle?) {
>         Log.i(TAG, "called onCreate")
>         super.onCreate(savedInstanceState)
>         setContentView(R.layout.activity_main)
>         window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
>         mOpenCvCameraView =
>             findViewById<View>(R.id.opencv_camera_view) as CameraBridgeViewBase
>         mOpenCvCameraView!!.visibility = SurfaceView.VISIBLE
>         mOpenCvCameraView!!.setCvCameraViewListener(this)
>     }
>     
>     public override fun onPause() {
>         super.onPause()
>         if (mOpenCvCameraView != null) mOpenCvCameraView!!.disableView()
>     }
>     
>     public override fun onResume() {
>         super.onResume()
>         if (!OpenCVLoader.initDebug()) {
>             Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization")
>             OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback)
>         } else {
>             Log.d(TAG, "OpenCV library found inside package. Using it!")
>             mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS)
>         }
>     }
>     
>     override fun getCameraViewList(): MutableList<CameraBridgeViewBase?> {
>         return Collections.singletonList(mOpenCvCameraView)
>     }
>     
>     public override fun onDestroy() {
>         super.onDestroy()
>         if (mOpenCvCameraView != null) mOpenCvCameraView!!.disableView()
>     }
>     
>     override fun onCameraViewStarted(width: Int, height: Int) {}
>     override fun onCameraViewStopped() {}
>     override fun onCameraFrame(inputFrame: CvCameraViewFrame): Mat {
>         // You can simply do this but this class is for C++ based camera processing template
>         // return inputFrame.gray()
>         frame = inputFrame.rgba()
>         grayscale(frame.nativeObjAddr, gray.nativeObjAddr)
>         return gray
>     }
>     
>     external fun grayscale(addrIn: Long, addrOut: Long)
>     
>     companion object {
>         // Used to load the 'camera_test' library on application startup.
>         private const val TAG = "OCVSample::Activity"
>         init {
>             System.loadLibrary("camera_test")
>         }
>     }
> 
> }

*Add your model to the project:*

- Convert your model to ORT format (will be documented later)

- Create an assets folder under app/src/main

- Add the model files into assets folder

*Setup ONNXRuntime Model Loading:*

- Add this function to your class in order to find filepath of asset in Android env:

> fun assetFilePath(context: Context, assetName: String): String? {  
>     val file: File = File(context.getFilesDir(), assetName)  
>     if (file.exists() && file.length() > 0) {  
>         Log.d("filetag", "Filepath: " + file.absolutePath)  
>         return file.absolutePath  
>  }  
>     try {  
>         context.getAssets().open(assetName).use { `is` ->  
>  FileOutputStream(file).use { os ->  
>  val buffer = ByteArray(4 * 1024)  
>                 var read: Int  
>                 while (`is`.read(buffer).also { read = it } != -1) {  
>                     os.write(buffer, 0, read)  
>                 }  
>                 os.flush()  
>             }  
>  return file.absolutePath  
>  }  
>  } catch (e: IOException) {  
>         Log.e("filetag", "Error process asset $assetName to file path")  
>     }  
>     return null  
> }

- In the OnCreate function, add this and modify model name "barcode.ort" with your own:

> val ortPath = assetFilePath(this, "barcode.ort");  
> 
> val env = OrtEnvironment.getEnvironment()  
> val opts = SessionOptions()  
> opts.setOptimizationLevel(OptLevel.BASIC_OPT)  
> opts.setSessionLogLevel(OrtLoggingLevel.ORT_LOGGING_LEVEL_VERBOSE)  
> val session: OrtSession = env.createSession(ortPath, opts)

*Setup The Emulator:*

- .

*Run:*

- .
