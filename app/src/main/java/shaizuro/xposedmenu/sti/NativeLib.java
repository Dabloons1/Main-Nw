package shaizuro.xposedmenu.sti;

import android.util.Log;

public class NativeLib {
    private static final String TAG = "NativeLib";
    private static boolean isLoaded = false;

    static {
        try {
            System.loadLibrary("xposedmodmenu");
            isLoaded = true;
            Log.i(TAG, "Native library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load native library: " + e.getMessage());
            isLoaded = false;
        }
    }

    /**
     * Initialize the native mod menu
     * This will start the hooking thread and setup ImGui
     */
    public static native void initModMenu();

    /**
     * Show or hide the mod menu
     * @param visible true to show, false to hide
     */
    public static native void setMenuVisible(boolean visible);

    /**
     * Check if the menu has been initialized
     * @return true if initialized, false otherwise
     */
    public static native boolean isMenuInitialized();

    /**
     * Check if native library is loaded
     * @return true if loaded successfully
     */
    public static boolean isLibraryLoaded() {
        return isLoaded;
    }

    /**
     * Initialize the menu safely with error handling
     */
    public static void safeInit() {
        if (!isLoaded) {
            Log.e(TAG, "Cannot initialize - native library not loaded");
            return;
        }

        try {
            initModMenu();
            Log.i(TAG, "Mod menu initialization started");
        } catch (Exception e) {
            Log.e(TAG, "Error initializing mod menu: " + e.getMessage());
            e.printStackTrace();
        }
    }

    /**
     * Toggle menu visibility
     */
    public static void toggleMenu() {
        if (!isLoaded) return;
        
        try {
            // For now, just show the menu - you can add state tracking if needed
            setMenuVisible(true);
        } catch (Exception e) {
            Log.e(TAG, "Error toggling menu: " + e.getMessage());
        }
    }
}

