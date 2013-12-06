import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class TSP {
	private static boolean benchmark;
	private static int noNeighbours = 15;
	private static int NO_RUNS = 50;
	private static int TIME_UNTIL_EXIT = 1800;
	private double[][] coordinates;
	private Integer[][] distances;
	private List<List<Short>> neighbours;

	public TSP() {
		long deadline = System.currentTimeMillis() + TIME_UNTIL_EXIT;

		try {
			coordinates = readCoordinates();
		} catch (IOException e) {
			e.printStackTrace();
		}

		calculateAllDistances();
		Short[] route = new NearestNeighbour(distances, neighbours, 0).run();
		route = new TwoOpt(distances, neighbours).run(route, deadline);
		if (System.currentTimeMillis() < (deadline - 600))
			route = new TwoOpt(distances, neighbours).run(route, deadline);
		int minDistance = calculateTotalDistance(route);

		for (int i = 0; i < NO_RUNS; i++) {
			if (System.currentTimeMillis() >= (deadline - 600))
				break;
			Short[] newRoute = new NearestNeighbour(distances, neighbours, 35).run();
			newRoute = new TwoOpt(distances, neighbours).run(newRoute, deadline);
			newRoute = new TwoOpt(distances, neighbours).run(newRoute, deadline);
			int newDistance = calculateTotalDistance(newRoute);
			if (newDistance < minDistance) {
				minDistance = newDistance;
				route = newRoute;
			}
		}

		if (benchmark) {
			System.out.println(calculateTotalDistance(route));
		} else {
			for (int i = 0; i < route.length; i++) {
				System.out.println(route[i]);
			}
		}
	}

	private void createNearestNeighbours() {
		if (distances.length <= noNeighbours)
			noNeighbours = distances.length - 1;

		List<Short> rangeList = new ArrayList<Short>();
		for (short i = 0; i < distances.length; i++) {
			rangeList.add(i);
		}
		neighbours = new ArrayList<List<Short>>(distances.length);
		for (int i = 0; i < distances.length; i++) {
			List<Short> neighbourList = new ArrayList<Short>(rangeList);
			final Integer[] comp = distances[i];
			Collections.sort(neighbourList, new Comparator<Short>() {
				public int compare(Short a, Short b) {
					return comp[a].compareTo(comp[b]);
				}
			});
			neighbours.add(new ArrayList<Short>(neighbourList.subList(0, noNeighbours)));
		}
	}

	private int calculateTotalDistance(Short[] route) {
		int sum = distances[route[route.length - 1]][route[0]];
		for (int i = 0; i < route.length - 1; i++) {
			sum += distances[route[i]][route[(i + 1)]];
		}
		return sum;
	}

	private void calculateAllDistances() {
		distances = new Integer[coordinates.length][];
		for (int i = 0; i < coordinates.length; i++) {
			distances[i] = new Integer[coordinates.length];
			distances[i][i] = Integer.MAX_VALUE;
			for (int j = 0; j < i; j++) {
				int dist = calculateDistance(coordinates[i], coordinates[j]);
				distances[i][j] = dist;
				distances[j][i] = dist;
			}
		}
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
