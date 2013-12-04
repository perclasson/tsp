import java.util.List;

public class NearestNeighbour {

	private Integer[][] distances;
	private List<List<Short>> neighbours;

	public NearestNeighbour(Integer[][] distances, List<List<Short>> neighbours) {
		this.distances = distances;
		this.neighbours = neighbours;
	}

	public Short[] run() {
		Short[] newRoute = new Short[distances.length];
		boolean[] visited = new boolean[distances.length];

		int firstVertex = 0;
		newRoute[0] = (short) firstVertex;
		visited[firstVertex] = true;

		for (int i = 0; i < newRoute.length - 1; i++) {
			newRoute[i + 1] = findNearestNeighbour(newRoute[i], visited);
		}

		return newRoute;
	}

	private short findNearestNeighbour(int from, boolean[] visited) {
		int minCost = Integer.MAX_VALUE;
		short nearest = 0;
		for (short to = 0; to < distances.length; to++) {
			int cost = distances[from][to];
			if (!visited[to] && cost < minCost) {
				minCost = cost;
				nearest = to;
			}
		}
		visited[nearest] = true;
		return nearest;
	}
	private short findNearestNeighbourFail(int from, boolean[] visited) {
		for (Short to : neighbours.get(from)) {
			if (!visited[to]) {
				visited[to] = true;
				return to;
			}
		}

		int minCost = Integer.MAX_VALUE;
		short nearest = 0;
		for (short to = 0; to < distances.length; to++) {
			int cost = distances[from][to];
			if (!visited[to] && cost < minCost) {
				minCost = cost;
				nearest = to;
			}
		}
		visited[nearest] = true;
		return nearest;
	}
}
