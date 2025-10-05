package shaizuro.xposedmenu.sti;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        TextView statusText = findViewById(R.id.statusText);
        
        // Check if Xposed Framework is active
        if (isXposedActive()) {
            statusText.setText("✓ Module Active in LSPosed");
            statusText.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
        } else {
            statusText.setText("⚠ LSPosed Framework not detected");
            statusText.setTextColor(getResources().getColor(android.R.color.holo_orange_dark));
        }
        
        // Show native library status
        TextView nativeStatus = findViewById(R.id.nativeLibStatus);
        if (nativeStatus != null) {
            if (NativeLib.isLibraryLoaded()) {
                nativeStatus.setText("✓ Native Library: Loaded");
                nativeStatus.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
            } else {
                nativeStatus.setText("✗ Native Library: Not Loaded");
                nativeStatus.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
            }
        }
        
        // Test button
        Button testButton = findViewById(R.id.testModMenuButton);
        if (testButton != null) {
            testButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    showModuleInfo();
                }
            });
        }
    }
    
    private boolean isXposedActive() {
        // This returns false in normal app context
        // Returns true only when loaded by LSPosed
        return false;
    }
    
    private void showModuleInfo() {
        StringBuilder info = new StringBuilder();
        info.append("=== LSPosed Mod Menu ===\n\n");
        info.append("Package: ").append(getPackageName()).append("\n");
        info.append("Native Library: ").append(NativeLib.isLibraryLoaded() ? "Loaded ✓" : "Not Loaded ✗").append("\n");
        info.append("Architecture: ").append(android.os.Build.SUPPORTED_ABIS[0]).append("\n");
        info.append("Android: ").append(android.os.Build.VERSION.RELEASE).append(" (SDK ").append(android.os.Build.VERSION.SDK_INT).append(")\n\n");
        info.append("Features:\n");
        info.append("• ImGui Menu System\n");
        info.append("• Native C++ Hooking\n");
        info.append("• Il2Cpp Support\n");
        info.append("• OpenGL Rendering\n\n");
        info.append("Usage:\n");
        info.append("1. Enable module in LSPosed\n");
        info.append("2. Select target app/game\n");
        info.append("3. Restart target app\n");
        info.append("4. Menu appears automatically\n\n");
        info.append("Configure target in MainHook.java");
        
        new android.app.AlertDialog.Builder(this)
            .setTitle("Module Information")
            .setMessage(info.toString())
            .setPositiveButton("OK", null)
            .show();
    }
}
