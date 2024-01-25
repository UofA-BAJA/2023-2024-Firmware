import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
 
public class App extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        try{
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("fxml/Main.fxml"));
            Parent root = fxmlLoader.load();
            SceneController sceneController = fxmlLoader.getController();
            sceneController.initialize();

            Scene scene = new Scene(root);
            String css = getClass().getResource("application.css").toExternalForm();
            scene.getStylesheets().add(css);
            
            primaryStage.setScene(scene);
            primaryStage.show();
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}