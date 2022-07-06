#include "snake.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

snake_t *create_snake(size_t player_num, coordinate_t head)
{
    snake_t *snake = (snake_t *) malloc(sizeof(snake_t));
    snake->length = 2;
    snake->player_num = player_num;
    memcpy(&snake->head, &head, sizeof(head));
    snake->status = PLAYING;

    if (pthread_mutex_init(&snake->lock, NULL) != 0)
    {
        print_error("Failed to initialise mutex for snake");
        return NULL;
    }

    snake->tail.direction = snake->head.direction;

    switch (snake->tail.direction)
    {
        case UP:
            snake->tail.x = snake->head.x;
            snake->tail.y = snake->head.y + 1;
            break;

        case LEFT:
            snake->tail.x = snake->head.x + 1;
            snake->tail.y = snake->head.y;
            break;

        case DOWN:
            snake->tail.x = snake->head.x;
            snake->tail.y = snake->head.y - 1;
        
        case RIGHT:
            snake->tail.x = snake->head.x - 1;
            snake->tail.y = snake->head.y;
            break;
        default: break; // Required fro no warnings
    }

    return snake;
}

void destroy_snake(snake_t *snake)
{
    pthread_mutex_destroy(&snake->lock);
    free(snake);
    snake = NULL;
}

void change_snake_direction(snake_t *snake, direction_t direction)
{
    pthread_mutex_lock(&snake->lock);
    snake->head.direction = direction;
    pthread_mutex_unlock(&snake->lock);
}

void move_snake(snake_t *snake)
{
    size_t body_len = snake->length - 2;

    pthread_mutex_lock(&snake->lock);

    // Updating body by shifting
    memmove(&snake->body[1], &snake->body[0], body_len * sizeof(coordinate_t));
    snake->body[0] = snake->head;

    // Updating head
    switch (snake->head.direction)
    {
        case UP:
            snake->head.y = snake->head.y - 1;
            break;

        case LEFT:
            snake->head.x = snake->head.x - 1;
            break;

        case DOWN:
            snake->head.y = snake->head.y + 1;
            break;
        
        case RIGHT:
            snake->head.x = snake->head.x + 1;
            break;
        
        default: break; // Required for no warnings
    }

    // Updating tail
    snake->tail = snake->body[body_len];

    pthread_mutex_unlock(&snake->lock);
}

void feed_snake(snake_t *snake)
{
    pthread_mutex_lock(&snake->lock);

    ++snake->length;
    snake->body[snake->length-3] = snake->tail;
    
    // Adding new tail  by updating old
    switch (snake->tail.direction)
    {
        case UP:
            snake->tail.y = snake->tail.y + 1;
            break;

        case LEFT:
            snake->tail.x = snake->tail.x + 1;
            break;

        case DOWN:
            snake->tail.y = snake->tail.y - 1;
            break;

        case RIGHT:
            snake->tail.x = snake->tail.x - 1;
            break;

        default: break; // Required fro no warnings
    }

    pthread_mutex_unlock(&snake->lock);
}