import java.util.HashMap;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.layout.AnchorPane;

public class SceneController{

    @FXML private AnchorPane home;
    @FXML private AnchorPane setup;
    @FXML private AnchorPane rpm;
    @FXML private AnchorPane imu;
    @FXML private AnchorPane brake;
    
    private AnchorPane active = new AnchorPane();
    private HashMap<String, AnchorPane> sceneMap = new HashMap<>();

    public void initialize(){
        sceneMap.put("Home", home);
        sceneMap.put("Setup", setup);
        sceneMap.put("RPM", rpm);
        sceneMap.put("IMU", imu);
        sceneMap.put("Brake", brake);
    }

    private void activate(String name){
        AnchorPane sceneToActivate = sceneMap.get(name);

        if(sceneToActivate == active){
            System.out.println("Scene Already active");
            return;
        }
        sceneToActivate.setVisible(true);
        sceneToActivate.setDisable(false);
        active.setVisible(false);
        active.setDisable(true);
        active = sceneToActivate;
    }
    
    public void switchToSceneHome(ActionEvent e){
        System.out.println("Attempting to switch to home");
        activate("Home");
    }
    public void switchToSceneSetup(ActionEvent e){
        System.out.println("Attempting to switch to setup");
        activate("Setup");
    }
    public void switchToSceneRPM(ActionEvent e){
        System.out.println("Attempting to switch to RPM");
        activate("RPM");
    }
    public void switchToSceneIMU(ActionEvent e){
        System.out.println("Attempting to switch to IMU");
        activate("IMU");
    }
    public void switchToSceneBrake(ActionEvent e){
        System.out.println("Attempting to switch to brake");
        activate("Brake");
    }
}