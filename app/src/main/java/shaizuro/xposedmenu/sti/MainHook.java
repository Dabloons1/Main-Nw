package shaizuro.xposedmenu.sti;

import android.app.Activity;
import android.widget.Toast;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class MainHook implements IXposedHookLoadPackage {
    
    private static final String TAG = "LSPosedModMenu";
    
    // Target the Soul Strike game package
    private static final String TARGET_PACKAGE = "com.com2usholdings.soulstrike.android.google.global.normal";
    
    // Static initializer
    static {
        XposedBridge.log(TAG + ": ===== MODULE LOADED =====");
        android.util.Log.i(TAG, "===== MODULE LOADED =====");
    }
    
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        // Skip system packages to avoid issues
        if (lpparam.packageName.startsWith("android") || 
            lpparam.packageName.startsWith("com.android") ||
            lpparam.packageName.equals("system")) {
            return;
        }
        
        // Check if we should hook this package
        boolean shouldHook = TARGET_PACKAGE.isEmpty() || lpparam.packageName.equals(TARGET_PACKAGE);
        
        if (!shouldHook) {
            return;
        }
        
        XposedBridge.log(TAG + ": Hooking package: " + lpparam.packageName);
        android.util.Log.i(TAG, "Hooking package: " + lpparam.packageName);
        
        try {
            // Hook Activity.onCreate to initialize our native menu
            XposedHelpers.findAndHookMethod(Activity.class, "onCreate", android.os.Bundle.class, new XC_MethodHook() {
                @Override
                protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                    Activity activity = (Activity) param.thisObject;
                    String packageName = activity.getPackageName();
                    
                    XposedBridge.log(TAG + ": Activity created in: " + packageName);
                    android.util.Log.i(TAG, "Activity created in: " + packageName);
                    
                    // Start overlay service for LSPosed
                    try {
                        XposedBridge.log(TAG + ": Starting overlay service...");
                        
                        Intent serviceIntent = new Intent(activity, OverlayService.class);
                        activity.startService(serviceIntent);
                        
                        Toast.makeText(activity, 
                            "LSPosed Mod Menu Active\nPackage: " + packageName, 
                            Toast.LENGTH_LONG).show();
                            
                        XposedBridge.log(TAG + ": Overlay service started successfully");
                    } catch (Exception e) {
                        XposedBridge.log(TAG + ": Error starting overlay service: " + e.getMessage());
                        android.util.Log.e(TAG, "Error starting overlay service", e);
                    }
                }
            });
            
            XposedBridge.log(TAG + ": Successfully hooked " + lpparam.packageName);
            
        } catch (Exception e) {
            XposedBridge.log(TAG + ": Hook failed: " + e.getMessage());
            android.util.Log.e(TAG, "Hook failed", e);
            e.printStackTrace();
        }
    }
}