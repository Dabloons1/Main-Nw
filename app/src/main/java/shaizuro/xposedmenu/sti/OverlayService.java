package shaizuro.xposedmenu.sti;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class OverlayService extends Service {
    private static final String TAG = "OverlayService";
    private WindowManager windowManager;
    private View overlayView;
    private boolean isOverlayVisible = false;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "OverlayService created");
        createOverlay();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "OverlayService started");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createOverlay() {
        windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        
        // Create a simple overlay layout
        overlayView = new LinearLayout(this);
        overlayView.setBackgroundColor(0x80000000); // Semi-transparent black
        overlayView.setPadding(20, 20, 20, 20);
        
        // Add title
        TextView title = new TextView(this);
        title.setText("LSPosed Mod Menu");
        title.setTextColor(0xFFFFFFFF);
        title.setTextSize(18);
        ((LinearLayout) overlayView).addView(title);
        
        // Add status text
        TextView status = new TextView(this);
        status.setText("Status: Active");
        status.setTextColor(0xFF00FF00);
        status.setTextSize(14);
        ((LinearLayout) overlayView).addView(status);
        
        // Add toggle button
        Button toggleButton = new Button(this);
        toggleButton.setText("Toggle Menu");
        toggleButton.setOnClickListener(v -> {
            toggleOverlay();
        });
        ((LinearLayout) overlayView).addView(toggleButton);
        
        // Add close button
        Button closeButton = new Button(this);
        closeButton.setText("Close");
        closeButton.setOnClickListener(v -> {
            hideOverlay();
        });
        ((LinearLayout) overlayView).addView(closeButton);
        
        // Set up window parameters
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
            PixelFormat.TRANSLUCENT
        );
        
        params.gravity = Gravity.TOP | Gravity.START;
        params.x = 50;
        params.y = 100;
        
        // Add the overlay to the window manager
        try {
            windowManager.addView(overlayView, params);
            isOverlayVisible = true;
            Log.i(TAG, "Overlay added successfully");
        } catch (Exception e) {
            Log.e(TAG, "Failed to add overlay: " + e.getMessage());
        }
    }
    
    private void toggleOverlay() {
        if (isOverlayVisible) {
            hideOverlay();
        } else {
            showOverlay();
        }
    }
    
    private void showOverlay() {
        if (!isOverlayVisible && overlayView != null) {
            try {
                WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                    WindowManager.LayoutParams.WRAP_CONTENT,
                    WindowManager.LayoutParams.WRAP_CONTENT,
                    WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
                    WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                    PixelFormat.TRANSLUCENT
                );
                params.gravity = Gravity.TOP | Gravity.START;
                params.x = 50;
                params.y = 100;
                
                windowManager.addView(overlayView, params);
                isOverlayVisible = true;
                Log.i(TAG, "Overlay shown");
            } catch (Exception e) {
                Log.e(TAG, "Failed to show overlay: " + e.getMessage());
            }
        }
    }
    
    private void hideOverlay() {
        if (isOverlayVisible && overlayView != null) {
            try {
                windowManager.removeView(overlayView);
                isOverlayVisible = false;
                Log.i(TAG, "Overlay hidden");
            } catch (Exception e) {
                Log.e(TAG, "Failed to hide overlay: " + e.getMessage());
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (overlayView != null && isOverlayVisible) {
            windowManager.removeView(overlayView);
        }
        Log.i(TAG, "OverlayService destroyed");
    }
}
