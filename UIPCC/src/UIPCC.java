import java.io.File;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Vector;
import java.util.Arrays;

public class UIPCC {
	public static void main(String[] args) throws Exception {

		long startTime = System.currentTimeMillis(); // record the total running time
		boolean debugMode = false;
		String[] configPara;
		// for testing
		if (debugMode) {
			configPara = new String[]{
					"D:\\experiment\\data\\dataset#1\\rawData\\rtMatrix.txt",
					"D:\\experiment\\data\\dataset#1\\generatedData\\RT\\density_0.20\\",
					"resultFolder\\rtResult_0.20.txt",
					"339",
					"5825",
					"0.20",
					"2",
					"false"};
		}
		else {
			configPara = args;
		}
		
	    // generate resultFolder to put results
	    File file = new File("resultFolder");
	    if(!file.exists()) {
	    	file.mkdir();
	    }
		                        
		runPredictMethods(configPara);		
		long endTime = System.currentTimeMillis(); 
		System.out.println(String.format("All done. Total running time: %.4f sec\n\n", 
				(endTime - startTime) / 1000.0));	
	}
	
	public static void runPredictMethods(String[] para) throws Exception {
		// parse the config parameters
		String dataMatrix = para[0]; // e.g., "data/rtMatrix.txt"; 
		String dataFolder = para[1]; // the generated data folder of training and testing data
	    String outPath = para[2]; // output filepath of the results
		int userNumber = Integer.parseInt(para[3]);	// e.g., 339 
		int itemNumber = Integer.parseInt(para[4]);	// e.g 5825
	    double density = Double.parseDouble(para[5]); // matrix density, e.g., 0.3
	    final int ITERATIONS = Integer.parseInt(para[6]); // rounds for each density
	    boolean saveTimeInfo = Boolean.parseBoolean(para[7]); // whether to save the runing time info
	    
		// five methods (UMEAN, IMEAN, UPCC, IPCC, UIPCC) 
		// and five metrics (MAE, NMAE, RMSE, MRE, NPRE)
	    // run for ITERATIONS rounds
	    double[][][] evalResults = new double[5][5][ITERATIONS];
	    double[][] timeResults = new double[5][ITERATIONS];
	        
	    System.out.println(String.format("Run density = %.2f for %d rounds...", 
	    		density, ITERATIONS));
		double[][] originalMatrix = Utilities.readMatrix(dataMatrix, userNumber, itemNumber);
		for (int k = 0; k < ITERATIONS; k++) {
			String traindata = String.format("%sround_%02d_train.txt", dataFolder, k + 1);
			String testdata = String.format("%sround_%02d_test.txt", dataFolder, k + 1);
			double[][] indexMatrix = Utilities.readMatrix(traindata, userNumber, itemNumber);			
			double[][] removedMatrix = new double[userNumber][itemNumber];
			for (int i = 0; i < userNumber; i++) {
				for (int j = 0; j < itemNumber; j++) {
					if (indexMatrix[i][j] == 1)
						removedMatrix[i][j] = originalMatrix[i][j];
					else
						removedMatrix[i][j] = -2;
				}
			}
			
			long iterStartTime = System.currentTimeMillis();
			double[][] tmpResults = predict(originalMatrix, removedMatrix, density, testdata);			
			long iterEndTime = System.currentTimeMillis();
			System.out.println(String.format("%d-round done. Running time: %.4f sec", 
					k + 1, (iterEndTime - iterStartTime) / 1000.0)); 
			for (int i = 0; i < tmpResults.length; i++) {
				for (int j = 0; j < tmpResults[0].length - 1; j++) {
					evalResults[i][j][k] = tmpResults[i][j];
				}
				timeResults[i][k] = tmpResults[i][tmpResults[0].length - 1];
			}
			 
		}

		saveEvalResult(evalResults, outPath); // write the evaluation results into file
		if (saveTimeInfo) {
			double[][] avgTimeResults = new double[timeResults.length][2];
			for (int i = 0; i < timeResults.length; i++) {
				avgTimeResults[i] = Utilities.getMeanAndStd(timeResults[i]);
			}
			// write the running time into file
			saveTimeResult(avgTimeResults, outPath + "_time.txt"); 
		}
	}
	
	public static void saveEvalResult(double[][][] resultMatrix, String outFile) {
		File f = new File(outFile);
		if(f.exists()) f.delete();
		
		double[][] avgEvalResults = new double[5][5];
		double[][] stdEvalResults = new double[5][5];
		for (int i = 0; i < resultMatrix.length; i++) {
			for (int j = 0; j < resultMatrix[0].length; j++) {
				double[] stat = Utilities.getMeanAndStd(resultMatrix[i][j]);
				avgEvalResults[i][j] = stat[0];
				stdEvalResults[i][j] = stat[1];
			}
		}
		
		String line = "Metrics: MAE\t| NMAE\t| RMSE\t| MRE\t| NPRE\r\n";
		line += "======================================================";
		String[] approach = {"UMEAN\r\n", "IMEAN\r\n", "UPCC\r\n", "IPCC\r\n", "UIPCC\r\n"};
		for (int i = 0; i < avgEvalResults.length; i++) {
			if (i < approach.length) {
				line += "\r\n" + approach[i];
			}
			else line += "Approach" + i + ":\r\n";
			line += "Avg:\t";
			for (int j = 0; j < avgEvalResults[0].length; j++) {
				line += String.format("%.4f\t", avgEvalResults[i][j]);
			}
			line += "\r\nStd:\t";
			for (int j = 0; j < avgEvalResults[0].length; j++) {
				line += String.format("%.4f\t", stdEvalResults[i][j]);
			}
			line += "\r\n";
			line += "======================================================";					 
		}
		
		line += "\r\n\r\nDetailed results for " + resultMatrix[0][0].length + " rounds:\r\n";
		line += "======================================================";
		for (int i = 0; i < avgEvalResults.length; i++) {
			if (i < approach.length) {
				line += "\r\n" + approach[i];
			}
			else line += "Approach" + i + ":\r\n";
			line += "======================================================\r\n";
			for (int k = 0; k < resultMatrix[0][0].length; k++) {
				for (int j = 0; j < avgEvalResults[0].length; j++) {
					line += String.format("%.4f\t", resultMatrix[i][j][k]);
				}
				line += "\r\n";
			}
			line += "\r\n";			
		}
		Utilities.writeFile(outFile, line + "\r\n");
    }
	
	public static void saveTimeResult(double[][] resultMatrix, String outFile) {
		File f = new File(outFile);
		if(f.exists()) f.delete();
		
		String line = "";
		String[] approach = {"UMEAN\r\n", "IMEAN\r\n", "UPCC\r\n", "IPCC\r\n", "UIPCC\r\n"};
		for (int i = 0; i < resultMatrix.length; i++) {
			if (i < approach.length) {
				line += approach[i];
			}
			else line += "Approach" + i + ":\r\n";
			line += String.format("Avg:\t%.4f\r\nStd:\t%.4f\r\n\r\n", 
					resultMatrix[i][0], resultMatrix[i][1]);			 
		}
		Utilities.writeFile(outFile, line);
	}
	
	public static double[][] predict(double[][] originalMatrix, 
			double[][] removedMatrix, double density, String testdata) {
		
		double[][] originalMatrixT = Utilities.matrixTransfer(originalMatrix);
		double[][] removedMatrixT = Utilities.matrixTransfer(removedMatrix);
		double[][] tmpResults = new double[5][6]; // the last column to store time info		
		double mae;
		double nmae;
		double rmse;
		double[] mre;
		long startTimer;
		long endTimer;
		
		// get the testing matrix
		double[][] testIndexMatrix = Utilities.readMatrix(testdata, 
				originalMatrix.length, originalMatrix[0].length);
		double[][] testMatrix = new double[originalMatrix.length][originalMatrix[0].length];
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if (testIndexMatrix[i][j] == 1)
					testMatrix[i][j] = originalMatrix[i][j];
				else
					testMatrix[i][j] = -1;
			}
		}
		double[][] testMatrixT = Utilities.matrixTransfer(testMatrix);
		
		// UMEAN
		startTimer = System.currentTimeMillis();
		double[] umean = getUMean(removedMatrix);
		double[][] predictedMatrix = UMEAN(originalMatrix, removedMatrix, umean);
		endTimer = System.currentTimeMillis();
		mae = MAE(testMatrix, removedMatrix, predictedMatrix);
		nmae = NMAE(testMatrix, removedMatrix, predictedMatrix);
		rmse = RMSE(testMatrix, removedMatrix, predictedMatrix);
		mre = MRE(testMatrix, removedMatrix, predictedMatrix);
		System.out.println(String.format("UMEAN done. " +
				"[MAE: %.4f, NMAE: %.4f, RMSE: %.4f, MRE: %.4f, NPRE: %.4f]", 
				mae, nmae, rmse, mre[0], mre[1]));
		tmpResults[0][0] = mae; 
		tmpResults[0][1] = nmae;
		tmpResults[0][2] = rmse; 
		tmpResults[0][3] = mre[0]; 
		tmpResults[0][4] = mre[1];
		tmpResults[0][5] = (endTimer - startTimer) / 1000.0;
		
		// IMEAN 
		startTimer = System.currentTimeMillis();
		double[] imean = getUMean(removedMatrixT);
		predictedMatrix = UMEAN(originalMatrixT, removedMatrixT, imean);
		endTimer = System.currentTimeMillis();
		mae = MAE(testMatrixT, removedMatrixT, predictedMatrix);
		nmae = NMAE(testMatrixT, removedMatrixT, predictedMatrix);
		rmse = RMSE(testMatrixT, removedMatrixT, predictedMatrix);
		mre = MRE(testMatrixT, removedMatrixT, predictedMatrix);
		System.out.println(String.format("IMEAN done. " +
				"[MAE: %.4f, NMAE: %.4f, RMSE: %.4f, MRE: %.4f, NPRE: %.4f]", 
				mae, nmae, rmse, mre[0], mre[1]));
		tmpResults[1][0] = mae; 
		tmpResults[1][1] = nmae;
		tmpResults[1][2] = rmse; 
		tmpResults[1][3] = mre[0]; 
		tmpResults[1][4] = mre[1];
		tmpResults[1][5] = (endTimer - startTimer) / 1000.0;
		
		// UPCC
		startTimer = System.currentTimeMillis();
		double[][] predictedMatrixUPCC = UPCC(originalMatrix, removedMatrix, 
				umean, true, false, 20, 10);
		endTimer = System.currentTimeMillis();
		mae = MAE(testMatrix, removedMatrix, predictedMatrixUPCC);
		nmae = NMAE(testMatrix, removedMatrix, predictedMatrixUPCC);
		rmse = RMSE(testMatrix, removedMatrix, predictedMatrixUPCC);
		mre = MRE(testMatrix, removedMatrix, predictedMatrixUPCC);
		System.out.println(String.format("UPCC done. " +
				"[MAE: %.4f, NMAE: %.4f, RMSE: %.4f, MRE: %.4f, NPRE: %.4f]", 
				mae, nmae, rmse, mre[0], mre[1]));
		tmpResults[2][0] = mae;      
		tmpResults[2][1] = nmae;
		tmpResults[2][2] = rmse; 
		tmpResults[2][3] = mre[0]; 
		tmpResults[2][4] = mre[1];
		tmpResults[2][5] = (endTimer - startTimer) / 1000.0;
	
		// IPCC
		startTimer = System.currentTimeMillis();
		double[][] predictedMatrixIPCC = UPCC(originalMatrixT, removedMatrixT, 
				imean, true, false, 20, 10);
		endTimer = System.currentTimeMillis();
		mae = MAE(testMatrixT, removedMatrixT, predictedMatrixIPCC);
		nmae = NMAE(testMatrixT, removedMatrixT, predictedMatrixIPCC);
		rmse = RMSE(testMatrixT, removedMatrixT, predictedMatrixIPCC);
		mre = MRE(testMatrixT, removedMatrixT, predictedMatrixIPCC);
		System.out.println(String.format("IPCC done. " +
				"[MAE: %.4f, NMAE: %.4f, RMSE: %.4f, MRE: %.4f, NPRE: %.4f]", 
				mae, nmae, rmse, mre[0], mre[1]));
		tmpResults[3][0] = mae; 
		tmpResults[3][1] = nmae;
		tmpResults[3][2] = rmse; 
		tmpResults[3][3] = mre[0]; 
		tmpResults[3][4] = mre[1];
		tmpResults[3][5] = (endTimer - startTimer) / 1000.0;
		
		// UIPCC
		double[] mae2 = new double[11];
		double[] nmae2 = new double[11];
		double[] rmse2 = new double[11]; 
		double[] mre2 = new double[11];
		double[] ninetythPercent = new double[11];
		double combTime = 0;
		// determine the best parameter lambda
		for (int i = 0; i < 11; i++) {
			double lambda = (double) i/10.0;
			startTimer = System.currentTimeMillis();
			double[][] predictedMatrixUIPCC = UIPCCFunc(predictedMatrixUPCC, 
					predictedMatrixIPCC, lambda);
			endTimer = System.currentTimeMillis();
			combTime += (endTimer - startTimer) / 1000.0;
			mae2[i] =   MAE(testMatrix, removedMatrix, predictedMatrixUIPCC);
			nmae2[i] =   NMAE(testMatrix, removedMatrix, predictedMatrixUIPCC);
			rmse2[i] = RMSE(testMatrix, removedMatrix, predictedMatrixUIPCC);
			mre = MRE(testMatrix, removedMatrix, predictedMatrixUIPCC);
			mre2[i] = mre[0];
			ninetythPercent[i] = mre[1];
		}
		// the average time for computing the weighted sum of UPCC and IPCC
		combTime = combTime / 11; 
		
		double smallMAE = 100;
		double smallNMAE = 100;
		double smallRMSE = 100;
		double smallMRE = 100;
		double smallNinetyPercent = 100;
		for (int i = 0; i < mae2.length; i++) {
			if(mae2[i] < smallMAE) {
				smallMAE = mae2[i];
				smallNMAE = nmae2[i];
			    smallRMSE = rmse2[i];
			    smallMRE = mre2[i];
			    smallNinetyPercent = ninetythPercent[i];
			}
		}
				
		System.out.println(String.format("UIPCC done. " +
				"[MAE: %.4f, NMAE: %.4f, RMSE: %.4f, MRE: %.4f, NPRE: %.4f]", 
				smallMAE, smallNMAE, smallRMSE, smallMRE, smallNinetyPercent));
		tmpResults[4][0] = smallMAE; 
		tmpResults[4][1] = smallNMAE;
		tmpResults[4][2] = smallRMSE; 
		tmpResults[4][3] = smallMRE; 
		tmpResults[4][4] = smallNinetyPercent;
		// time for combination of UPCC and IPCC
		tmpResults[4][5] = tmpResults[3][5] + tmpResults[2][5] + combTime; 
		
		return tmpResults;
	}
	
	

	public static double[][] UIPCCFunc(double[][] predictedMatrixUPCC, 
			double[][] predictedMatrixIPCC, double lambda) {
		double[][]result = new double[predictedMatrixUPCC.length][predictedMatrixUPCC[0].length];
		for (int i = 0; i < predictedMatrixUPCC.length; i++) {
			for (int j = 0; j < predictedMatrixUPCC[0].length; j++) {
				result[i][j] = (double)(lambda * predictedMatrixUPCC[i][j] 
						+ (1 - lambda) * predictedMatrixIPCC[j][i]);
			}
		}
		return result;
	}	
	
	/**
	 * @param originalMatrix
	 * @param removedMatrix
	 * @param isSW
	 * @param swThreshold
	 */
	public static double[][] UPCC(double[][] originalMatrix, double[][] removedMatrix, 
			double[] umean, boolean isPCC, boolean isSW, int swThreshold, int topK) {
		
		double[][] predictedMatrix = new double[originalMatrix.length][originalMatrix[0].length];
		
		for (int i = 0; i < originalMatrix.length; i++) {
			// get the pcc values of the current user with all other users. 
					
			Map<Integer, Double> pcc = new HashMap<Integer, Double>();
			
			for (int j = 0; j < originalMatrix.length; j++) {
				// the same user. 
				if(i == j) continue;
				
				// the user has no ratings, no similarity computation. 
				if(umean[i] == -2 || umean[j] == -2) continue; 
				
				double pccValue = 0;
				if(isPCC)
					pccValue = getPCC(removedMatrix[i], removedMatrix[j], umean[i], 
							umean[j], isSW, swThreshold);
				else
					pccValue = getVSS(removedMatrix[i], removedMatrix[j], isSW, swThreshold);
				
				// find similar users, 
				if(pccValue>0)	pcc.put(j, pccValue);
			}
			//System.out.println(i + "\t" + pcc.size());
			Map<Integer, Double> sortedPcc = sortByValue(pcc);
			
			// predict values for each items in the line. 
			for (int j = 0; j < originalMatrix[0].length; j++) {
				// not removed entry, no need to predict. 
				if(removedMatrix[i][j] != -2) continue; 
				
				// no original value for making evaluation, no need to predict. 
				if(originalMatrix[i][j] < 0) continue;
				
				int k = 0;
				double pccAll = 0; 
				double predictedValue = 0;
				Iterator<Entry<Integer, Double>> it = sortedPcc.entrySet().iterator();
				while(k < topK && it.hasNext()){
					
					Entry<Integer, Double> en = it.next();
					int userID = en.getKey();
					
					// if the similar user does not use this item previously, can not be used. 
					if(removedMatrix[userID][j] == -2 || removedMatrix[userID][j] == -1) continue;
					
					double userPCCValue = (double)en.getValue();
					//System.out.println(userPCCValue + "\t");
					pccAll += userPCCValue;
					k++;
					
					predictedValue += (userPCCValue) * (originalMatrix[userID][j] - umean[userID]);
					//System.out.println(userPCCValue);
				}
				
				// no similar users, use umean. 
				if(pccAll == 0) {
					predictedValue = umean[i];
				} else{ 
					predictedValue = predictedValue/pccAll + umean[i];
				}
				
				
				// will become worst, no need. 
				if(predictedValue <= 0) predictedValue = umean[i];
			
			
				predictedMatrix[i][j] = (double)predictedValue;
			}
		}
		return predictedMatrix;
	}
		
	public static Map<Integer, Double> sortByValue(Map<Integer, Double> map) {
		List<Entry<Integer, Double>> list = new LinkedList<Entry<Integer, Double>>(map.entrySet());
		Collections.sort(list, new Comparator<Entry<Integer, Double>>() {
			public int compare(Entry<Integer, Double> o1, Entry<Integer, Double> o2) {
				return ((Comparable<Double>) ((Entry<Integer, Double>) (o1)).getValue())
						.compareTo(((Entry<Integer, Double>) (o2)).getValue());
			}
		});
		Collections.reverse(list);
		// logger.info(list);
		Map<Integer, Double> result = new LinkedHashMap<Integer, Double>();
		for (Iterator<Entry<Integer, Double>> it = list.iterator(); it.hasNext();) {
			Entry<Integer, Double> entry = it.next();
			result.put(entry.getKey(), entry.getValue());
		}
		return result;
	}
	
	
	public static double[][] UMEAN(double[][] originalMatrix, double[][] removedMatrix, 
			double[] umean) {
		double[][] predictedMatrix = new double[originalMatrix.length][originalMatrix[0].length];
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				// predict the remove entry and the original entry is not null. 
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1) {
					if(umean[i] == -2) predictedMatrix[i][j] = -2;
					else predictedMatrix[i][j] = (double)umean[i];
				}
			}
		}
		return predictedMatrix;
	}
	
	
	/**
	 * two vectors, and two means of the vectors.
	 * isWS: whether enable the significant weight. 
	 * swPercent: the threshold of the significant weight.  
	 *  
	 * @return
	 * -2 indicates this pcc value has problem and can't be used.
	 */
	public static double getPCC(double[] u1, double[] u2, double mean1, double mean2, 
			boolean isSW, int swThreshold) {
		// get the index of the common rated items.
		Vector<Integer> commonRatedKey = new Vector<Integer>();
		
		for (int i = 0; i < u1.length; i++) {
			if(u1[i] >= 0 && u2[i] >= 0) {
				commonRatedKey.add(i);
			}
		}
		
		// no common rate items. 
		if(commonRatedKey.size() == 0 || commonRatedKey.size() == 1) 
			return -2;
		
		
		double pcc = 0;
		double upperAll = 0;
		double downAll1 = 0;
		double downAll2 = 0;

		for (int i = 0; i < commonRatedKey.size(); i++) {
			int key = commonRatedKey.get(i);
			double value1 = u1[key];
			double value2 = u2[key];
			
			double temp1 = value1 - mean1;
			double temp2 = value2 - mean2;
			
			if(temp1 < 0.00001 && temp1 > 0) temp1 = 0.00001;
			if(temp2 < 0.00001 && temp2 > 0) temp2 = 0.00001;

			if(temp1 > -0.00001 && temp1 < 0) temp1 = -0.00001;
			if(temp2 > -0.00001 && temp2 < 0) temp2 = -0.00001;
			
			upperAll += temp1 * temp2;
			downAll1 += temp1 * temp1;
			downAll2 += temp2 * temp2;
		}
		
		double downValue = Math.sqrt(downAll1 * downAll2);
		
		if(downValue == 0) 
			return -2;
		
		pcc = upperAll / downValue;
		
		//use significant weight to avoid the over estimation problem.
		// 10 is a parameter, which can be set.
		if(isSW && commonRatedKey.size() < swThreshold) {
			pcc = pcc * commonRatedKey.size() / swThreshold;
		}
		return pcc;
	}
		
	public static double getVSS(double[] u1, double[] u2, boolean isSW, int swThreshold){
		// get the index of the common rated items.
		Vector<Integer> commonRatedKey = new Vector<Integer>();
		
		for (int i = 0; i < u1.length; i++) {
			if(u1[i] >= 0 && u2[i] >= 0) {
				commonRatedKey.add(i);
			}
		}
		
		// no common rate items. 
		if(commonRatedKey.size() == 0 || commonRatedKey.size() == 1) 
			return -2;
		
		
		double vss = 0;
		double upperAll = 0;
		double downAll1 = 0;
		double downAll2 = 0;

		for (int i = 0; i < commonRatedKey.size(); i++) {
			int key = commonRatedKey.get(i);
			double value1 = u1[key];
			double value2 = u2[key];
			
			if(value1 < 0.00001 && value1 > 0) value1 = 0.00001;
			if(value1 > -0.00001 && value1 < 0) value1 = -0.00001;
			
			if(value2 < 0.00001 && value2 > 0) value2 = 0.00001;
			if(value2 > -0.00001 && value2 < 0) value2 = -0.00001;
			
			upperAll += value1 * value2;
			downAll1 += value1 * value1;
			downAll2 += value2 * value2;
		}
		
		double downValue = Math.sqrt(downAll1 * downAll2);
		
		if(downValue == 0) 
			return -2;
		
		vss = upperAll / downValue;
		
		//use significant weight to avoid the over estimation problem.
		// 10 is a parameter, which can be set.
		if(isSW && commonRatedKey.size() < swThreshold) {
			vss = vss * commonRatedKey.size() / swThreshold;
		}
		return vss;
	}
		
	public static double MAE(double[][] originalMatrix, double[][] removedMatrix, 
			double[][] predictedMatrix) {
		double allMAE = 0;
		double number = 0;
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1 
						&& predictedMatrix[i][j] != -2) {
					allMAE += Math.abs(predictedMatrix[i][j] - originalMatrix[i][j]);
					number ++;
				}
			}
		}
		return allMAE/number;
	}
	
	public static double NMAE(double[][] originalMatrix, double[][] removedMatrix, 
			double[][] predictedMatrix){
		double allMAE = 0;
		double allOrigin = 0;
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1 
						&& predictedMatrix[i][j] != -2) {
					allMAE += Math.abs(predictedMatrix[i][j] - originalMatrix[i][j]);
					allOrigin += originalMatrix[i][j];
				}
			}
		}
		return allMAE/allOrigin;
	}
	
	public static double RMSE(double[][] originalMatrix, double[][] removedMatrix, 
			double[][] predictedMatrix) {
		double allRMSE = 0;
		double number = 0;
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1 
						&& predictedMatrix[i][j] != -2) {
					allRMSE += (predictedMatrix[i][j] - originalMatrix[i][j])
							* (predictedMatrix[i][j] - originalMatrix[i][j]);
					number ++;
				}
			}
		}
		return Math.sqrt(allRMSE/number);
	}
	
	public static double[] MRE(double[][] originalMatrix, double[][] removedMatrix, 
			double[][] predictedMatrix){
		int number = 0;
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1 
						&& predictedMatrix[i][j] != -2) {
					number ++;
				}
			}
		}
		double[] MREMatrix = new double[number];
		number=0;
		for (int i = 0; i < originalMatrix.length; i++) {
			for (int j = 0; j < originalMatrix[0].length; j++) {
				if(removedMatrix[i][j] == -2 && originalMatrix[i][j] != -1 
						&& predictedMatrix[i][j] != -2) {
					MREMatrix[number] = Math.abs(predictedMatrix[i][j] - originalMatrix[i][j])
							/ originalMatrix[i][j];
					number ++;
				}
			}
		}
		double[] result = new double[2];
		result[0] = median(MREMatrix);
		result[1] = ninetythPercent(MREMatrix);
		return result;
	}	
	
	public static double median(double a[]) {
		double[] b = new double[a.length];
		System.arraycopy(a, 0, b, 0, b.length);
		Arrays.sort(b);

		if (a.length % 2 == 0) {
		return (b[(b.length / 2) - 1] + b[b.length / 2]) / 2.0;
		} else {
		return b[b.length / 2];
		}
	}
	
	public static double ninetythPercent(double a[]) {
		double[] b = new double[a.length];
		System.arraycopy(a, 0, b, 0, b.length);
		Arrays.sort(b);
		return b[(int) (b.length * 0.9)];
	}
		
	public static double[] getUMean(double[][] removedMatrix){
		double[] umean = new double[removedMatrix.length];
		int[] uNumber = new int[removedMatrix.length];
		
		for (int i = 0; i < removedMatrix.length; i++) {
			umean[i]=0;
			uNumber[i]=0;
			for (int j = 0; j < removedMatrix[0].length; j++) {
				// exclude the null entries (-1) and the removed entries (-2). 
				if(removedMatrix[i][j] < 0) continue;
				
				umean[i] += removedMatrix[i][j];
				uNumber[i]++;
				
			}
		}
		
		for (int i = 0; i < umean.length; i++) {
			if(uNumber[i] ==0) umean[i] = -2;
			else umean[i] /= uNumber[i];
		}
		
		return umean;
	}
	
}
