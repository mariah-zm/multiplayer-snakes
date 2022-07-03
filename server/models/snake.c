#include "snake.h"

snake_t *create_snake(int player_num, coordinate_t const *head)
{
    snake_t *snake = malloc(sizeof(snake_t));
    snake->player_num = player_num;
    snake->length = 2;
    snake->head = *head;
    snake->status = PLAYING;

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
        
        default:
            snake->tail.x = snake->head.x - 1;
            snake->tail.y = snake->head.y;
            break;
    }

    return snake;
}

void destroy_snake(snake_t *snake)
{
    free(snake);
    snake = NULL;
}

void change_snake_direction(snake_t *snake, direction_t direction)
{
    snake->head.direction = direction;
}

void move_snake(snake_t *snake)
{
    size_t body_len = snake->length - 2;

    // Updating tail
    if (body_len > 0)
        snake->tail = snake->body[body_len-1];
    else 
        snake->tail = snake->head;

    // Updating body
    for (size_t idx = body_len-1; idx > 0; --idx)
        snake->body[idx] = snake->body[idx-1];

    // Updating coordinate of front of body
    snake->body[0] = snake->head;

    // Updating head
    switch (snake->head.direction)
    {
        case UP:
            snake->head.y = snake->head.y + 1;
            break;

        case LEFT:
            snake->head.x = snake->head.x + 1;
            break;

        case DOWN:
            snake->head.y = snake->head.y - 1;
        
        default:
            snake->head.x = snake->head.x - 1;
            break;
    }
}

void feed_snake(snake_t *snake)
{
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
        
        default:
            snake->tail.x = snake->tail.x - 1;
            break;
    }
}