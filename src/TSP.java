import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class TSP {
	private double[][] coordinates;
	private int[][] distances;
	private static boolean benchmark;

	public TSP() {
		long deadline = System.currentTimeMillis();

		try {
			coordinates = readCoordinates();
		} catch (IOException e) {
			e.printStackTrace();
		}

		distances = calculateAllDistances();

		Short[] route = new NearestNeighbour(distances).run();
		route = new TwoOpt(distances).run(route, deadline + 1500);

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.length; i++) {
				System.out.println(route[i]);
			}
		}
	}

	private int calculateTotalDistance(Short[] route) {
		int sum = distances[route[route.length - 1]][route[0]];
		for (int i = 0; i < route.length - 1; i++) {
			sum += distances[route[i]][route[(i + 1)]];
		}
		return sum;
	}

	private int[][] calculateAllDistances() {
		int[][] distances = new int[coordinates.length][];
		for (short i = 0; i < coordinates.length; i++) {
			distances[i] = new int[coordinates.length];
			for (short j = 0; j < i; j++) {
				int dist = calculateDistance(coordinates[i], coordinates[j]);
				distances[i][j] = dist;
				distances[j][i] = dist;
			}
		}
		return distances;
	}

	private int calculateDistance(double[] i, double[] j) {
		return (int) Math.round(Math.sqrt(Math.pow(i[0] - j[0], 2) + Math.pow(i[1] - j[1], 2)));
	}

	private double[][] readCoordinates() throws IOException {
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		double[][] data;
		int n = Integer.parseInt(br.readLine());
		data = new double[n][2];
		for (int i = 0; i < n; i++) {
			String[] input = br.readLine().split(" ");
			data[i][0] = Double.parseDouble(input[0]);
			data[i][1] = Double.parseDouble(input[1]);
		}
		return data;
	}

	public static void main(String[] args) {
		if (args.length > 0 && args[0].equals("benchmark")) {
			benchmark = true;
		}
		new TSP();
	}
}
