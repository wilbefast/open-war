/**
 * @author william
 * @created 17-Aug-2012
 */

package system;

import com.jme3.app.SimpleApplication;
import com.jme3.system.AppSettings;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Main
{
    public static void main(String[] args) 
    {
      // Create application
      SimpleApplication app = new MonkeyWar();
      // Configure application
      AppSettings settings = new AppSettings(true);
      settings.setFrameRate(60);
      settings.setTitle("Monkey War");
      app.setSettings(settings);
      // Configure logger
      Logger.getLogger( "" ).setLevel( Level.WARNING );
      // Launch application
      app.start();
    }
}
