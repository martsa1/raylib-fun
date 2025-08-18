from __future__ import annotations

from dataclasses import dataclass
import typing as t

MapT = list[list[int]]


@dataclass
class Pos:
    x: int
    y: int
    dist: int = -1
    previous: Pos | None = None


MAP: MapT = [
    [1, 0, 0],
    [0, 1, 0],
    # [0, 1, 0],
    [0, 0, 0],
    [0, 0, 0],
]

START = Pos(1, 0)
END = Pos(0, 1)


def can_move(target: Pos, map: MapT) -> bool:
    y_size = len(map)
    x_size = len(map[0])
    if target.x < 0 or target.x >= x_size:
        return False
    if target.y < 0 or target.y >= y_size:
        return False
    if map[target.y][target.x] == 1:
        return False

    return True


def find_neighbours(cur_pos: Pos, map: MapT) -> t.Iterator[Pos]:
    cardinals = (
        # North
        Pos(cur_pos.x , cur_pos.y - 1, cur_pos.dist + 1, cur_pos),
        # South
        Pos(cur_pos.x , cur_pos.y + 1, cur_pos.dist + 1, cur_pos),
        # East
        Pos(cur_pos.x - 1, cur_pos.y, cur_pos.dist + 1, cur_pos),
        # West
        Pos(cur_pos.x + 1, cur_pos.y, cur_pos.dist + 1, cur_pos),
    )
    for c in cardinals:
        if not can_move(c, map):
            continue
        yield c


def main() -> None:
    iter = 0

    to_visit = [START]
    visited: dict[tuple[int, int], Pos] = {}
    # visited = []
    print("   x: 0, 1, 2")
    for x in range(len(MAP)):
        print(f"y: {x} {MAP[x]}")

    while to_visit:
        if iter > 1_000:
            raise Exception("something fucked up...")

        next = to_visit.pop()
        if next.x == END.x and next.y == END.y:
            print(f"Destination reached: {next}")
            break

        print(f"visiting {next}")
        for neighbour in find_neighbours(next, MAP):
            neighbour_pos = neighbour.x, neighbour.y
            if neighbour_pos not in visited:
                to_visit.append(neighbour)
            # Don't revisit neighbours if we've been there before for less distance.
            elif visited[neighbour_pos].dist > neighbour.dist:
                    to_visit.append(neighbour)
            else:
                print(f"Skipping neighbour: {neighbour_pos} for distance {neighbour.dist}, already visited for dist: {visited[neighbour_pos].dist}")


        pos = (next.x, next.y)
        if pos not in visited:
            print(f"Storing entry for new pos {pos}")
            visited[pos] = next
            continue
        if next.dist < visited[pos].dist:
            print(f"Found new path to {pos} for dist {next.dist} (prev. {visited[pos].dist})")
            visited[pos] = next
            continue
        else:
            print(f"not updating {pos}, already found a shorter route.")

    path = [next]
    while True:
        if next.previous:
            next = next.previous
            path.append(next)
        else:
            break

    path = path[::-1]

    for i in path:
        print(f"{i.x, i.y}")


if __name__ == "__main__":
    main()
