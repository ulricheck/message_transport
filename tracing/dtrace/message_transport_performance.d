#!/usr/sbin/dtrace -v -s

#pragma D option quiet

dtrace:::BEGIN
{
    printf("#Start Capturing MessageTransport Statistics, CTRL+C to show results\n");

}

messagetransport*:::message-published
{
    message_transport_publish[arg0, copyinstr(arg1)] = timestamp;
}

messagetransport*:::message-callback
{
    @times_topic[copyinstr(arg1)] = sum(timestamp - message_transport_publish[arg0, copyinstr(arg1)]);
    @counts_topic[copyinstr(arg1)] = count();
    @avgtime_message[copyinstr(arg1)] = avg(timestamp - message_transport_publish[arg0, copyinstr(arg1)]);
}


END
{
        printf("#Time spent for tranporting message per topic (microseconds)\n");
        printf(">times_topic<");
        normalize(@times_topic,1000);
        printa(@times_topic);

        printf("#Number of messages processed per topic\n");
        printf(">counts_topic<");
        printa(@counts_topic);

        printf("#Time spent on delivering to topic (microseconds)\n");
        printf(">avgtime_message<");
        normalize(@avgtime_message,1000);
        printa(@avgtime_message);

}
