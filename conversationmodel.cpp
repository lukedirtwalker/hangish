/*

Hanghish
Copyright (C) 2015 Daniele Rogora

This file is part of Hangish.

Hangish is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Hangish is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>

*/


#include "conversationmodel.h"

ConversationModel::ConversationModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

void ConversationModel::addConversation(Conversation c)
{
    conversations.append(c);
}

void ConversationModel::addSentMessage(QString convId, Event evt)
{
    qDebug() << "TODO: implement me!";
}

void ConversationModel::addOutgoingMessage(QString convId, Event evt)
{
    qDebug() << "TODO: implement outgoing message";
}

void ConversationModel::addEventToConversation(QString convId, Event e)
{
    int i=0;
    for (i=0; i<conversations.size(); i++)
        if (conversations.at(i).id==convId)
            break;

    conversations[i].events.append(e);
    if (convId == id) {
        //It's the active one, should update model
        QString snd = getSenderName(e.sender.chat_id, conversations[i].participants);
        //if (snd == myself.chat_id)
        //    sndName = "Io";
        //else
            //sndName = getUserById(snd).first_name;
        QString text = "";
        foreach (EventValueSegment evs, e.value.segments)
            text += evs.value;
        QString fImage = "";
        foreach (EventAttachmentSegment eas, e.value.attachments)
            fImage += eas.fullImage;
        QString pImage = "";
        foreach (EventAttachmentSegment eas, e.value.attachments)
            pImage += eas.previewImage;

        //Show full date if event has not happened today; otherwise show only clock time
        QString ts_string;
        QDateTime now = QDateTime::currentDateTime();
        if (now.date().day() != e.timestamp.date().day())
            ts_string = e.timestamp.toString();
        else
            ts_string = e.timestamp.time().toString();
        addConversationElement(snd, e.sender.chat_id, ts_string, text, fImage, pImage);
    }
}

QHash<int, QByteArray> ConversationModel::roleNames() const {
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles.insert(IdRole, QByteArray("id"));
        roles.insert(SenderRole, QByteArray("sender"));
        roles.insert(SenderIdRole, QByteArray("senderId"));
        roles.insert(TextRole, QByteArray("msgtext"));
        roles.insert(FullImageRole, QByteArray("fullimage"));
        roles.insert(PreviewImageRole, QByteArray("previewimage"));
        roles.insert(TimestampRole, QByteArray("timestamp"));
        return roles;
}

QString ConversationModel::getCid()
{
    return id;
}

void ConversationModel::conversationLoaded()
{
    qDebug() << "Conv load finished! ";
    qDebug() << myList.size();
}

QString ConversationModel::getSenderName(QString chatId, QList<Participant> participants)
{
    foreach (Participant p, participants)
        if (p.user.chat_id == chatId)
            return p.user.first_name;
}

void ConversationModel::loadConversation(QString cId)
{
    id = cId;
    beginResetModel();
    qDeleteAll(myList);
    myList.clear();
    endResetModel();

    foreach (Conversation c, conversations) {
        if (c.id==cId) {
            name = c.name;
            id = c.id;
            foreach (Event e, c.events)
            {
                QString snd = getSenderName(e.sender.chat_id, c.participants);
                //if (snd == myself.chat_id)
                //    sndName = "Io";
                //else
                    //sndName = getUserById(snd).first_name;
                QString text = "";
                foreach (EventValueSegment evs, e.value.segments)
                    text += evs.value;
                QString fImage = "";
                foreach (EventAttachmentSegment eas, e.value.attachments)
                    fImage += eas.fullImage;
                QString pImage = "";
                foreach (EventAttachmentSegment eas, e.value.attachments)
                    pImage += eas.previewImage;

                //Show full date if event has not happened today; otherwise show only clock time
                QString ts_string;
                QDateTime now = QDateTime::currentDateTime();
                if (now.date().day() != e.timestamp.date().day())
                    ts_string = e.timestamp.toString();
                else
                    ts_string = e.timestamp.time().toString();
                addConversationElement(snd, e.sender.chat_id, ts_string, text, fImage, pImage);
            }
            return;
        }
    }
}

void ConversationModel::addConversationElement(QString sender, QString senderId, QString timestamp, QString text, QString fullimageUrl, QString previewimageUrl)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    //qDebug() << "Inserting " << sender << ": " << text;
    myList.append(new ConversationElement(sender, senderId, text, timestamp, fullimageUrl, previewimageUrl));
    endInsertRows();
}

int ConversationModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return myList.size();
}

QVariant ConversationModel::data(const QModelIndex &index, int role) const {
    //qDebug() << "REQUESTING DATA! " << role << " - " << index.row();
    ConversationElement * fobj = myList.at(index.row());
    if (role == SenderRole)
        return QVariant::fromValue(fobj->sender);
    else if (role == TextRole)
        return QVariant::fromValue(fobj->text);
    else if (role == SenderIdRole)
        return QVariant::fromValue(fobj->senderId);
    else if (role == TimestampRole)
        return QVariant::fromValue(fobj->timestamp);
    else if (role == FullImageRole)
        return QVariant::fromValue(fobj->fullimageUrl);
    else if (role == PreviewImageRole)
        return QVariant::fromValue(fobj->previewImageUrl);

    return QVariant();
}