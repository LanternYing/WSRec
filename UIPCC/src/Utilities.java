import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Random;

public class Utilities {
	
	public static double[][] readMatrix(String fileName, int userNumber, int itemNumber) {
		double[][] result = new double[userNumber][itemNumber];
		
		try {
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(fileName)));
			
			String line;
			int index = 0;
			while ((line = br.readLine()) != null) {
				if(index >= userNumber) break;
				String[] temp = line.split("\t");
				for (int j = 0; j < temp.length; j++) {
					result[index][j] = Double.parseDouble(temp[j]);
				}
				index++;
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (NumberFormatException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	public static double[][] matrixTransfer(double[][]originalMatrix) {
		double[][] result = new double[originalMatrix[0].length][originalMatrix.length];
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				result[j][i] = originalMatrix[i][j];
			}
		}
		return result;
	}
	
	public static void writeMatrix(double[][]matrix, String outFile){	
		File f = new File(outFile);
		if(f.exists()) f.delete();
		
		for (int i = 0; i < matrix.length; i++) {
			String line = "";
			for (int j = 0; j < matrix[0].length; j++) {
				line += String.format("%.4f", matrix[i][j]);		
				// avoid the last tab in the line. 
				if(j != matrix[0].length - 1)
					line += "\t";
			}
			writeFile(outFile, line + "\r\n");
		}	
	
	}
	
	public static void writeMatrix(double[] matrix,String outFile){	
		File f = new File(outFile);
		if(f.exists()) f.delete();
		String line = "";
		for (int i = 0; i < matrix.length; i++) {						
				line += String.format("%.4f", matrix[i]);		
				// avoid the last tab in the line. 
				if(i != matrix.length - 1)
					line += "\t";
		}	
		writeFile(outFile, line);	
	}
	
	public static void writeFile(String fileLocation, String content) {
        try {
            File file = new File(fileLocation);
            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
                        new FileOutputStream(file, true)));
            writer.write(content);
            writer.flush();
            writer.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
		
	public static double[][] removeEntry(double[][]originalMatrix, double density){	
		double[][] removedMatrix = new double[originalMatrix.length][originalMatrix[0].length];		
		Random r = new Random();
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				double value = r.nextDouble();
				if(value > density)							
					removedMatrix[i][j] = -2;
				else removedMatrix[i][j] = originalMatrix[i][j];
			}
		}
		return removedMatrix;
	}
	
	public static double[] getMeanAndStd(double[] dataArray) {
        try {
            double mean = 0, std = 0;
            for (int i = 0; i < dataArray.length; i++) {
            	mean += dataArray[i];
            }
            mean = mean / dataArray.length;
            for (int i = 0; i < dataArray.length; i++) {
            	std += (dataArray[i] - mean) * (dataArray[i] - mean);
            }
            std = Math.sqrt(std / dataArray.length);
            return new double[] {mean, std};
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

}
